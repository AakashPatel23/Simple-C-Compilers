//
//  RegAlloc.cpp
//  uscc
//
//  Implements a register allocator based on the
//  RegAllocBasic allocator from LLVM.
//---------------------------------------------------------
//  Portions of the code in this file are:
//  Copyright (c) 2003-2014 University of Illinois at
//  Urbana-Champaign.
//  All rights reserved.
//
//  Distributed under the University of Illinois Open Source
//  License.
//---------------------------------------------------------
//  Remaining code is:
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "llvm/CodeGen/Passes.h"
#include "../lib/CodeGen/AllocationOrder.h"
#include "../lib/CodeGen/LiveDebugVariables.h"
#include "../lib/CodeGen/RegAllocBase.h"
#include "../lib/CodeGen/Spiller.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/CodeGen/CalcSpillWeights.h"
#include "llvm/CodeGen/LiveIntervalAnalysis.h"
#include "llvm/CodeGen/LiveRangeEdit.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStackAnalysis.h"
#include "llvm/CodeGen/MachineBlockFrequencyInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/PassAnalysisSupport.h"
#undef DEBUG
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include <cstdlib>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>

using namespace llvm;

#define DEBUG_TYPE "regalloc"

static FunctionPass* createUSCCRegisterAllocator();

static RegisterRegAlloc usccRegAlloc("uscc", "USCC register allocator",
    createUSCCRegisterAllocator);

size_t NUM_COLORS = 4;

namespace {
  struct CompSpillWeight {
    bool operator()(LiveInterval *A, LiveInterval *B) const {
      return A->weight < B->weight;
    }
  };
}

namespace {

  /// RAUSCC allocator pass
  class RAUSCC : public MachineFunctionPass, public RegAllocBase {
    // context
    MachineFunction *MF;

    std::unordered_map<LiveInterval *, std::unordered_set<LiveInterval *>> interferenceGraph;
    std::vector<LiveInterval *> liveIntervals;
    std::vector<LiveInterval *> stack;
    bool spillOccurred = true;

    // state
    std::unique_ptr<Spiller> SpillerInstance;

    // Scratch space.  Allocated here to avoid repeated malloc calls in
    // selectOrSplit().
    BitVector UsableRegs;

    public:
    RAUSCC();

    /// Return the pass name.
    const char* getPassName() const override {
      return "Basic Register Allocator";
    }

    /// RAUSCC analysis usage.
    void getAnalysisUsage(AnalysisUsage &AU) const override;

    void releaseMemory() override;

    Spiller &spiller() override { return *SpillerInstance; }

    void enqueue(LiveInterval *LI) override {
      stack.push_back(LI);
    }

    LiveInterval *dequeue() override {
      if (stack.empty()) {
        return nullptr;
      }
      LiveInterval *top = stack.back();
      stack.pop_back();
      return top;
    }

    unsigned selectOrSplit(LiveInterval &VirtReg,
        SmallVectorImpl<unsigned> &SplitVRegs) override {
      llvm::llvm_unreachable_internal("This function is not used in RAUSCC!");
      return ~0u;
    }


    /// Perform register allocation.
    bool runOnMachineFunction(MachineFunction &mf) override;

    // Helper for spilling all live virtual registers currently unified under preg
    // that interfere with the most recently queried lvr.  Return true if spilling
    // was successful, and append any new spilled/split intervals to splitLVRs.
    bool spillInterferences(LiveInterval &VirtReg, unsigned PhysReg,
        SmallVectorImpl<unsigned> &SplitVRegs);

    void initGraph();
    void simplifyGraph();
    unsigned allocation();
    static char ID;
  };

  char RAUSCC::ID = 0;

} // end anonymous namespace

RAUSCC::RAUSCC(): MachineFunctionPass(ID) {
  initializeLiveDebugVariablesPass(*PassRegistry::getPassRegistry());
  initializeLiveIntervalsPass(*PassRegistry::getPassRegistry());
  initializeSlotIndexesPass(*PassRegistry::getPassRegistry());
  initializeRegisterCoalescerPass(*PassRegistry::getPassRegistry());
  initializeMachineSchedulerPass(*PassRegistry::getPassRegistry());
  initializeLiveStacksPass(*PassRegistry::getPassRegistry());
  initializeMachineDominatorTreePass(*PassRegistry::getPassRegistry());
  initializeMachineLoopInfoPass(*PassRegistry::getPassRegistry());
  initializeVirtRegMapPass(*PassRegistry::getPassRegistry());
  initializeLiveRegMatrixPass(*PassRegistry::getPassRegistry());
}

void RAUSCC::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesCFG();
  AU.addRequired<AliasAnalysis>();
  AU.addPreserved<AliasAnalysis>();
  AU.addRequired<LiveIntervals>();
  AU.addPreserved<LiveIntervals>();
  AU.addPreserved<SlotIndexes>();
  AU.addRequired<LiveDebugVariables>();
  AU.addPreserved<LiveDebugVariables>();
  AU.addRequired<LiveStacks>();
  AU.addPreserved<LiveStacks>();
  AU.addRequired<MachineBlockFrequencyInfo>();
  AU.addPreserved<MachineBlockFrequencyInfo>();
  AU.addRequiredID(MachineDominatorsID);
  AU.addPreservedID(MachineDominatorsID);
  AU.addRequired<MachineLoopInfo>();
  AU.addPreserved<MachineLoopInfo>();
  AU.addRequired<VirtRegMap>();
  AU.addPreserved<VirtRegMap>();
  AU.addRequired<LiveRegMatrix>();
  AU.addPreserved<LiveRegMatrix>();
  MachineFunctionPass::getAnalysisUsage(AU);
}

void RAUSCC::releaseMemory() {
  SpillerInstance.reset();
  liveIntervals.clear();
  interferenceGraph.clear();
  stack.clear();
}


// Spill or split all live virtual registers currently unified under PhysReg
// that interfere with VirtReg. The newly spilled or split live intervals are
// returned by appending them to SplitVRegs.
bool RAUSCC::spillInterferences(LiveInterval &VirtReg, unsigned PhysReg,
    SmallVectorImpl<unsigned> &SplitVRegs) {
  // Record each interference and determine if all are spillable before mutating
  // either the union or live intervals.
  SmallVector<LiveInterval*, 8> Intfs;

  // Collect interferences assigned to any alias of the physical register.
  for (MCRegUnitIterator Units(PhysReg, TRI); Units.isValid(); ++Units) {
    LiveIntervalUnion::Query &Q = Matrix->query(VirtReg, *Units);
    Q.collectInterferingVRegs();
    if (Q.seenUnspillableVReg())
      return false;
    for (unsigned i = Q.interferingVRegs().size(); i; --i) {
      LiveInterval *Intf = Q.interferingVRegs()[i - 1];
      if (!Intf->isSpillable() || Intf->weight > VirtReg.weight)
        return false;
      Intfs.push_back(Intf);
    }
  }
  DEBUG(dbgs() << "spilling " << TRI->getName(PhysReg) <<
      " interferences with " << VirtReg << "\n");
  assert(!Intfs.empty() && "expected interference");
  llvm::errs() << "Spilling "; llvm::errs().flush(); VirtReg.dump();
  // Spill each interfering vreg allocated to PhysReg or an alias.
  for (unsigned i = 0, e = Intfs.size(); i != e; ++i) {
    LiveInterval &Spill = *Intfs[i];

    // Skip duplicates.
    if (!VRM->hasPhys(Spill.reg))
      continue;

    // Deallocate the interfering vreg by removing it from the union.
    // A LiveInterval instance may not be in a union during modification!
    Matrix->unassign(Spill);

    // Spill the extracted interval.
    LiveRangeEdit LRE(&Spill, SplitVRegs, *MF, *LIS, VRM);
    spiller().spill(LRE);
  }
  return true;
}

bool RAUSCC::runOnMachineFunction(MachineFunction &mf) {
  DEBUG(dbgs() << "********** USCC REGISTER ALLOCATION **********\n"
      << "********** Function: "
      << mf.getName() << '\n');
  llvm::errs() << "********** USCC REGISTER ALLOCATION **********\n";
  std::string funcName(mf.getName());
  llvm::errs() << "********** Function: " << funcName << '\n';
  llvm::errs() << "NUM_COLORS=" << NUM_COLORS << '\n';
  MF = &mf;
  RegAllocBase::init(getAnalysis<VirtRegMap>(),
      getAnalysis<LiveIntervals>(),
      getAnalysis<LiveRegMatrix>());

  RegClassInfo.runOnMachineFunction(mf);
  calculateSpillWeightsAndHints(*LIS, *MF,
      getAnalysis<MachineLoopInfo>(),
      getAnalysis<MachineBlockFrequencyInfo>());

  SpillerInstance.reset(createInlineSpiller(*this, *MF, *VRM));

  spillOccurred = true;

  while (spillOccurred) {
    spillOccurred = false;
    for (unsigned i = 0, e = MRI->getNumVirtRegs(); i != e; i++) {
      unsigned Reg = TargetRegisterInfo::index2VirtReg(i);
      if (MRI->reg_nodbg_empty(Reg)) {
        continue;
      }
      LiveInterval *VirtReg = &LIS->getInterval(Reg);
      if (VRM->hasPhys(Reg)) {
        Matrix->unassign(*VirtReg);
        VRM->clearVirt(Reg);
      }
    }

    initGraph();
    simplifyGraph();
    allocation();
  }
  // Diagnostic output before rewriting
  DEBUG(dbgs() << "Post alloc VirtRegMap:\n" << *VRM << "\n");

  releaseMemory();
  return true;
}

// Build an interference graph
void RAUSCC::initGraph() {
  for (unsigned i = 0, e = MRI->getNumVirtRegs(); i != e; i++) {
    unsigned Reg = TargetRegisterInfo::index2VirtReg(i);
    if (MRI->reg_nodbg_empty(Reg)) {
      continue;
    }
    LiveInterval *VirtReg = &LIS->getInterval(Reg);
    interferenceGraph[VirtReg] = {};
  }

  for (auto &v1 : interferenceGraph) {
    for (auto &v2 : interferenceGraph) {
      if (v1.first->overlaps(*v2.first) && v1.first != v2.first) {
        v1.second.insert(v2.first);
      }
    }
  }
}

void RAUSCC::simplifyGraph() {
  while (!interferenceGraph.empty()) {
    LiveInterval* remove1 = nullptr;
    LiveInterval* remove2 = nullptr;
    for (auto &v : interferenceGraph) {
      if (v.second.size() < NUM_COLORS) {
        if (!remove1 || v.first->reg < remove1->reg) {
          remove1 = v.first;
        }
      }
      else if (!remove2 || v.first->weight < remove2->weight ||
               (v.first->weight == remove2->weight && v.first->reg < remove2->reg)) {
        remove2 = v.first;
      }
    }
    if (remove1) {
      llvm::errs() << "Found neighbors=" << interferenceGraph[remove1].size() << " for ";
      remove1->dump();
      llvm::errs() << "Removal: ";
      remove1->dump();
      for (auto *v : interferenceGraph[remove1]) {
        interferenceGraph[v].erase(remove1);
      }
      interferenceGraph.erase(remove1);
      enqueue(remove1);
    }
    else {
      llvm::errs() << "Spill candidate (neighbors=" << interferenceGraph[remove2].size() << ") pushed optimistically: ";
      remove2->dump();
      llvm::errs() << "Note: This node would have been spilled in Chaitin, but is pushed to stack optimistically.\nRemoval: ";
      remove2->dump();
      for (auto *v : interferenceGraph[remove2]) {
        interferenceGraph[v].erase(remove2);
      }
      interferenceGraph.erase(remove2);
      enqueue(remove2);
    }
  }
}

unsigned RAUSCC::allocation() {
  // Assign virtual registers to available physical registers.
  while (LiveInterval *virtReg = dequeue()) {
    Matrix->invalidateVirtRegs();

    SmallVector<unsigned, 4> splitVRegs;

    // Check for an available register in this class.
    // NOTE: This line crashes if RegClassInfo is not initialized in runOnMachineFunction!
    AllocationOrder Order(virtReg->reg, *VRM, RegClassInfo);
    unsigned availablePhysReg = 0;

    while ((availablePhysReg = Order.next())) {
      // Check for interference in PhysReg
      if (Matrix->checkInterference(*virtReg, availablePhysReg) == LiveRegMatrix::IK_Free) {
        // PhysReg is available, allocate it.
        errs() << "Assigning to physical register " <<
          TRI->getName(availablePhysReg) << ": " << *virtReg << "\n";
        break;
      }
    }

    if (availablePhysReg) {
      Matrix->assign(*virtReg, availablePhysReg);
    } else {
      // Step #3: Chaitin-Briggs algorithm
      // If some vertex cannot be colored, then pick an uncolored vertex to spill
      llvm::errs() << "Misspeculation in Chaitin-Briggs: spill happens for ";
      llvm::errs().flush();
      virtReg->dump();
      llvm::errs() << "Spilling "; 
      llvm::errs().flush(); 
      virtReg->dump();

      if (!virtReg->isSpillable())
        return ~0u;

      LiveRangeEdit LRE(virtReg, splitVRegs, *MF, *LIS, VRM);
      spiller().spill(LRE);

      for (auto reg : splitVRegs) {
        LiveInterval *splitVirtReg = &LIS->getInterval(reg);
        assert(!VRM->hasPhys(splitVirtReg->reg) && "Register already assigned");
        if (MRI->reg_nodbg_empty(reg)) {
          LIS->removeInterval(splitVirtReg->reg);
          continue;
        }
        errs() << "add new interval for spilling to the IG: " << *splitVirtReg << "\n";
        assert(TargetRegisterInfo::isVirtualRegister(splitVirtReg->reg) &&
            "expect split value in virtual register");
      }
      // Step #3: Chaitin-Briggs algorithm
      // If we spill, we must stop allocation and restart the main loop
      spillOccurred = true;
      break;
    }
  }
  return 0;
}

FunctionPass* createUSCCRegisterAllocator() {
  return new RAUSCC();
}
