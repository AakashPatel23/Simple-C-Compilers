#include "Graph.h"
#include <algorithm>

using namespace llvm::uscc;

GraphNode *Graph::getOrAddToGraph(llvm::Value *V) {
  GraphNode *Node = ValueMap[V];
  if (!Node) {
    Node = new GraphNode(V);
    ValueMap[V] = Node;
  }
  return Node;
}

bool Graph::eraseFromGraph(Value *V) {
  GraphNode *Node = ValueMap[V];
  if (!Node) {
    return false;
  }
  ValueMap.erase(V);
  delete Node;
  return true;
}

size_t Graph::eraseAllNodes() {
  size_t size = ValueMap.size();
  for (auto I : ValueMap) {
    delete I.second;
  }
  ValueMap.clear();
  return size;
}

std::vector<std::set<llvm::Value *>> Graph::getSCCs() {
  dfsLastDisc = 0;
  SCCVec.clear();
  resetAllNodes();
  for (auto p : ValueMap) {
    auto N = p.second;
    if (N->discovered == 0) {
      dfsFindScc(N);
    }
  }
  return SCCVec;
}

void Graph::resetAllNodes() {
  for (auto &I : ValueMap) {
    I.second->discovered = 0;
  }
}

void Graph::dfsFindScc(GraphNode *V) {
  V->low = V->discovered = ++dfsLastDisc;
  dfsStack.push(V);
  V->onStack = true;
  for (auto I = V->begin(), E = V->end(); I != E; ++I) {
    auto c = *I;
    if (c->discovered == 0) {
      dfsFindScc(c);
      V->low = std::min(c->low, V->low);
    } else if (c->onStack) {
      V->low = std::min(c->discovered, V->low);
    }
  }
  if (V->low == V->discovered) {
    SCCVec.push_back(std::set<Value *>());
    auto &newSCC = SCCVec.back();
    while (true) {
      auto t = dfsStack.top();
      dfsStack.pop();
      newSCC.insert(t->getValue());
      t->onStack = false;
      if (t == V) {
        break;
      }
    }
  }
}