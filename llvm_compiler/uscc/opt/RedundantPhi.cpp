#include "Graph.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"

namespace llvm {
  namespace uscc {
    namespace opt {

      class RedundantPhiRemoval : public FunctionPass {
        public:
          static char ID;
          RedundantPhiRemoval() : FunctionPass(ID) {}
          virtual bool runOnFunction(llvm::Function &F) override;
          void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
          bool removeRedundantPhis(std::set<PHINode*>& phiNodes);
          void processSCC(std::set<llvm::Value*>& scc);
          void replaceSCCByValue(std::set<llvm::Value*>& scc, llvm::Value* value);
      };



      char RedundantPhiRemoval::ID = 0;

      void RedundantPhiRemoval::getAnalysisUsage(llvm::AnalysisUsage &AU) const {
        AU.setPreservesCFG();
      }


      bool RedundantPhiRemoval::runOnFunction(Function &F) {
        std::set<PHINode*> nodes;
        for (auto &BB : F) {
          for (auto &I : BB) {
            if (isa<PHINode>(&I)) {
              nodes.insert(dyn_cast<PHINode>(&I));
            }
          }
        }

        return nodes.empty()? false : removeRedundantPhis(nodes);
      }


      bool RedundantPhiRemoval::removeRedundantPhis(std::set<PHINode*>& nodes) {
        if (nodes.empty()) {
          return false;
        }

        Graph *g = new Graph();

        for (auto phi : nodes) {
          g->getOrAddToGraph(phi);
        }

        for (auto phi : nodes) {
          GraphNode *node = g->getOrAddToGraph(phi);

          for (int i = 0; i < phi->getNumIncomingValues(); i++) {
            auto op = phi->getIncomingValue(i);
            if (isa<PHINode>(op)) {
              node->addEdge(g->getOrAddToGraph(op));
            }
          }
        }

        auto sccs = g->getSCCs();
        for (auto &scc : sccs) {
          processSCC(scc);
        }

        g->eraseAllNodes();
        delete g;
        return true;
      }

      void RedundantPhiRemoval::processSCC(std::set<Value*>& scc) {
        std::set<PHINode*> inner;
        std::set<Value*> outerOps;
        for (auto &node : scc) {
          auto *phi = dyn_cast<PHINode>(node);
          bool isInner = true;
          for (int i = 0; i < phi->getNumIncomingValues(); i++) {
            auto op = phi->getIncomingValue(i);
            if (!scc.count(op)) {
              outerOps.insert(op);
              isInner = false;
            }
          }
          if (isInner) {
            inner.insert(phi);
          }
        }
        if (outerOps.size() == 1) {
          replaceSCCByValue(scc, *outerOps.begin());
        }
        else if (outerOps.size() > 1) {
          removeRedundantPhis(inner);
        }
      }


      void RedundantPhiRemoval::replaceSCCByValue(std::set<Value*>& scc, Value *op) {
        std::vector<User*> users;
        for (auto &phi : scc) {
          for (auto use = phi->user_begin(); use != phi->user_end(); ++use) {
            if (scc.find(*use) == scc.end()) {
              users.push_back(*use);
            }
          }
        }
        for (auto &node : scc) {
          auto *phi = dyn_cast<PHINode>(node);
          phi->replaceAllUsesWith(op);
          phi->eraseFromParent();
        }
    }
  } // namespace opt
} // namespace uscc


FunctionPass *createRedundantPhiRemovalPass() {
  return new uscc::opt::RedundantPhiRemoval();
}

} // namespace llvm

