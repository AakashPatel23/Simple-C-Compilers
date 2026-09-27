
/**
 * USC Compiler
 *
 * An iterative forward available expressions analysis.
 */

#include "Passes.h"
#include "AvailableExpressions.h"
#include "llvm/IR/CFG.h"

using namespace std;
using namespace llvm;

bool enableAE;

namespace {
  // Helper for set intersection
  std::set<uint32_t> intersect(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    std::set<uint32_t> result;
    std::set_intersection(A.begin(), A.end(),
        B.begin(), B.end(),
        std::inserter(result, result.begin()));
    return result;
  }

  // Helper for set union (needed for transfer function)
  std::set<uint32_t> set_union(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    std::set<uint32_t> result = A;
    result.insert(B.begin(), B.end());
    return result;
  }

  // Helper for set difference (needed for transfer function)
  std::set<uint32_t> set_difference(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    std::set<uint32_t> result;
    std::set_difference(A.begin(), A.end(),
        B.begin(), B.end(),
        std::inserter(result, result.begin()));
    return result;
  }

  // Helper for comparing sets
  bool areSetsEqual(const std::set<uint32_t>& A, const std::set<uint32_t>& B) {
    return A == B;
  }

  // Helper for computing post order
  void computePostOrder(BasicBlock *entry, set<BasicBlock *> &visited, deque<BasicBlock *> &order) {
    visited.insert(entry);
    auto succItr = succ_begin(entry), end = succ_end(entry);
    for (; succItr != end; ++succItr) {
      if (!visited.count(*succItr))
        computePostOrder(*succItr, visited, order);
    }
    order.push_back(entry);
  }
}

char AvailableExpressions::ID = 0;
INITIALIZE_PASS(AvailableExpressions, "ae", "Available Expressions Analysis", true, true)

AvailableExpressions::AvailableExpressions() : FunctionPass(ID) {
  initializeAvailableExpressionsPass(*PassRegistry::getPassRegistry());
}

FunctionPass *llvm::createAEPass() {
  return new AvailableExpressions();
}

// Helper for computing IN set query for the basic block BB
std::vector<Instruction*> AvailableExpressions::getInSet(BasicBlock* BB) {
  std::vector<Instruction*> result;

  // 1. Find the IN set (of IDs) for the requested block
  if (bb2In.find(BB) == bb2In.end()) {
    // This block has no IN set (e.g., unreachable)
    return result;
  }

  // Get the set of uint32_t IDs
  const std::set<uint32_t>& inSetIDs = bb2In.at(BB);
  if (inSetIDs.empty()) {
    for (auto &Inst : *BB) {
      if (isa<BinaryOperator>(&Inst) || isa<CmpInst>(&Inst)) {
        result.push_back(&Inst);
      }
    }
  }
  else {
    // 2. Convert IDs back to Instruction*
    for (uint32_t id : inSetIDs) {
      // 3. Find Instruction* from id2Expr
      if (id2Expr[id] != nullptr) {
        // 4. Add to result vector
        result.push_back(id2Expr[id]);
      }
    }
  }

  // 5. Return the vector of available instructions
  return result;
}

void AvailableExpressions::dumpAvailableExpressions(Function &F, unsigned iterationCount) {
  llvm::outs() << "********** Available Expressions Information **********\n";
  llvm::outs() << "********** Function: " << F.getName() << ", analysis iterates " << iterationCount << " times\n";

  for (auto &bb : F) {
    llvm::outs() << bb.getName() << ":\n";
    llvm::outs() << "  GEN:";
    for (uint32_t id : this->bb2Gen[&bb]) {
      if (id < this->id2Expr.size() && this->id2Expr[id]) {
        llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
      } else {
        llvm::outs() << " [ID:" << id << "?]";
      }
    }
    llvm::outs() << "\n";

    llvm::outs() << "  KILL:";
    for (uint32_t id : this->bb2Kill[&bb]) {
      if (id < this->id2Expr.size() && this->id2Expr[id]) {
        llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
      } else {
        llvm::outs() << " [ID:" << id << "?]";
      }
    }
    llvm::outs() << "\n";

    // Print IN set
    llvm::outs() << "  IN:";
    if (this->bb2In.count(&bb)) {
      for (uint32_t id : this->bb2In[&bb]) {
        if (id < this->id2Expr.size() && this->id2Expr[id]) {
          llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
        } else {
          llvm::outs() << " [ID:" << id << "?]";
        }
      }
    }
    llvm::outs() << "\n";

    // Print OUT set
    llvm::outs() << "  OUT:";
    if (this->bb2Out.count(&bb)) {
      for (uint32_t id : this->bb2Out[&bb]) {
        if (id < this->id2Expr.size() && this->id2Expr[id]) {
          llvm::outs() << " [" << id << ": " << *this->id2Expr[id] << "]";
        } else {
          llvm::outs() << " [ID:" << id << "?]";
        }
      }
    }
    llvm::outs() << "\n";
  }
  llvm::outs() << "\n";
}


bool AvailableExpressions::runOnFunction(Function &F) {
  if (F.empty())
    return false;
  int iterationCount = 0;

  /* Step 1: Identify all expressions and create mappings */
  /* Iterate over all BB in F and all instructions in each BB */
  /* Check if type of I is binary op or comparison */
  /* Maps instruction to unique ID and vice versa */

  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {
      if (isa<BinaryOperator>(&I) || isa<CmpInst>(&I)) {
        expr2Id[&I] = id2Expr.size();
        universalSet.insert(id2Expr.size());
        id2Expr.push_back(&I);
      }
    }
  }

  /* Step 2: Calculate GEN and KILL sets */

  for (BasicBlock &BB : F) {
    std::set<uint32_t> gen;
    std::set<uint32_t> kill;

    for (Instruction &I : BB) {
      std::set<uint32_t> G;
      std::set<uint32_t> K;

      /* Fill out G as instruction ID */

      auto it = expr2Id.find(&I);
      if (it != expr2Id.end()) {
        G.insert(it->second);
      }

      /* Kill all instructions that have redefined operands or memory locations */

      if (!I.getType()->isVoidTy() &&  !isa<AllocaInst>(&I)) {
        Value *result = &I;
        for (uint32_t id : universalSet) {
          Instruction *expr = id2Expr[id];
          for (auto op = expr->op_begin(); op != expr->op_end(); ++op) {
            if (op->get() == result) {
              K.insert(id);
              break;
            }
          }
        }
      }
      else if (StoreInst *si = dyn_cast<StoreInst>(&I)) {
        for (uint32_t id : universalSet) {
          Instruction *expr = id2Expr[id];
          for (auto op = expr->op_begin(); op != expr->op_end(); ++op) {
            if (LoadInst *li = dyn_cast<LoadInst>(op->get())) {
              if (li->getPointerOperand() == si->getPointerOperand()) {
                K.insert(id);
                break;
              }
            }
          }
        }
      }

      /* GEN = G + (GEN - K) and KILL = K + (KILL - G) */

      gen = set_union(G, set_difference(gen, K));
      kill = set_union(K, set_difference(kill, G));
    }

    bb2Gen[&BB] = gen;
    bb2Kill[&BB] = kill;
  }

  /* Step 3: Initialize IN and OUT sets */

  for (BasicBlock &BB : F) {
    bb2In[&BB] = {};
    bb2Out[&BB] = &F.getEntryBlock() == &BB ? std::set<uint32_t>() : bb2Gen[&BB];
  }
  /* Step 4: Worklist algorithm */

  std::deque<BasicBlock*> worklist;
  std::set<BasicBlock*> visited;
  computePostOrder(&F.getEntryBlock(), visited, worklist);

  while (!worklist.empty()) {
    iterationCount++;

    /* Take BB from back of list because reverse postorder */

    BasicBlock *BB = worklist.back();
    worklist.pop_back();
    std::set<uint32_t> newIn;

    /* Meet Operator: Intersection of all predecessor OUT sets */

    for (auto pred = pred_begin(BB); pred != pred_end(BB); ++pred) {
      newIn = pred == pred_begin(BB) ? bb2Out[*pred] : intersect(newIn, bb2Out[*pred]);
    }
    bb2In[BB] = newIn;

    /* Transfer Function: OUT = GEN + (IN - KILL) */

    std::set<uint32_t> newOut = set_union(bb2Gen[BB], set_difference(bb2In[BB], bb2Kill[BB]));

    /* Adds blocks to worklist if propagation is needed */

    if (!areSetsEqual(newOut, bb2Out[BB])) {
      bb2Out[BB] = newOut;
      for (auto succ = succ_begin(BB); succ != succ_end(BB); ++succ) {
        worklist.push_front(*succ);
      }
    }
  }

  if (enableAE) {
    dumpAvailableExpressions(F, iterationCount);
  }

  return false;
}

// Helper function to use for CSE
bool AvailableExpressions::isAvailableAfter(Instruction &expr, Instruction &point) {
  if (expr2Id.find(&expr) == expr2Id.end()) {
    return false; // Not an expression we are tracking
  }
  BasicBlock* BB = point.getParent();
  if (!BB || bb2In.find(BB) == bb2In.end()) {
    return false; // Should not happen
  }

  std::set<uint32_t> ae = bb2In[BB];
  for (Instruction &I : *BB) {
    if (&I == &point) {
      return ae.count(expr2Id[&expr]);
    }

    /* Kill all instructions that have redefined operands or memory locations */

    std::set<uint32_t> K;
    if (!I.getType()->isVoidTy() &&  !isa<AllocaInst>(&I)) {
      Value *result = &I;
      for (uint32_t id : universalSet) {
        Instruction *expr = id2Expr[id];
        for (auto op = expr->op_begin(); op != expr->op_end(); ++op) {
          if (op->get() == result) {
            K.insert(id);
            break;
          }
        }
      }
    }
    else if (StoreInst *si = dyn_cast<StoreInst>(&I)) {
      for (uint32_t id : universalSet) {
        Instruction *expr = id2Expr[id];
        for (auto op = expr->op_begin(); op != expr->op_end(); ++op) {
          if (LoadInst *li = dyn_cast<LoadInst>(op->get())) {
            if (li->getPointerOperand() == si->getPointerOperand()) {
              K.insert(id);
              break;
            }
          }
        }
      }
    }

    ae = set_difference(ae, K);
    auto it = expr2Id.find(&I);
    if (it != expr2Id.end()) {
      ae.insert(it->second);
    }

  }
  return false;
}
