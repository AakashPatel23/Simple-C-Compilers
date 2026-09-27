#ifndef LLVM_USCC_GRAPH_H
#define LLVM_USCC_GRAPH_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include <set>
#include <stack>

namespace llvm {

namespace uscc {

class GraphNode;

class Graph {

  typedef std::map<const Value *, GraphNode *> ValueMapTy;

public:
  ~Graph() { eraseAllNodes(); }

  typedef ValueMapTy::iterator iterator;
  typedef ValueMapTy::const_iterator const_iterator;

  iterator begin() { return ValueMap.begin(); }
  iterator end() { return ValueMap.end(); }
  const_iterator begin() const { return ValueMap.begin(); }
  const_iterator end() const { return ValueMap.end(); }

  /// Return the graph node for the provides value.
  const GraphNode *operator[](const Value *V) const {
    const_iterator I = ValueMap.find(V);
    assert(I != ValueMap.end() && "Value not in graph!");
    return I->second;
  }

  /// Return the graph node for the provides value.
  GraphNode *operator[](const Value *V) {
    const_iterator I = ValueMap.find(V);
    assert(I != ValueMap.end() && "Value not in graph!");
    return I->second;
  }

  /// Add a new node of the provided value to the graph. If the node already
  /// exists, return that node instead.
  GraphNode *getOrAddToGraph(Value *V);

  /// Erase all nodes in the graphs.
  size_t eraseAllNodes();

  /// Remove the corresponding node of the provided value from the graph.
  bool eraseFromGraph(Value *V);

  /// Get the SCCs in the graph. The returned vector contains multiple sets.
  /// Each set represents a SCC in the graph
  std::vector<std::set<llvm::Value *>> getSCCs();

private:
  void dfsFindScc(GraphNode *V);

  void resetAllNodes();

  /// A map from Value* to GraphNode*.
  ValueMapTy ValueMap;

  std::stack<GraphNode *> dfsStack;

  std::vector<std::set<Value *>> SCCVec;

  int dfsLastDisc;
};

class GraphNode {

  friend class Graph;

public:
  /// \brief Creates a node for the specified Value.
  inline GraphNode(Value *V) : V(V), onStack(false), discovered(0), low(0) {}

  typedef std::vector<GraphNode *>::iterator iterator;
  typedef std::vector<GraphNode *>::const_iterator const_iterator;

  /// \brief Returns the Value.
  Value *getValue() const { return V; }

  inline iterator begin() { return Children.begin(); }
  inline iterator end() { return Children.end(); }
  inline const_iterator begin() const { return Children.begin(); }
  inline const_iterator end() const { return Children.end(); }
  inline bool empty() const { return Children.empty(); }
  inline unsigned getChildNum() const { return (unsigned)Children.size(); }
  inline operator Value *() { return getValue(); }

  GraphNode *operator[](unsigned i) const {
    assert(i < Children.size() && "Invalid index");
    return Children[i];
  }

  /// Remove all edges from this node
  void removeAllEdges() { Children.clear(); }

  /// Adds a edge from this node to C
  void addEdge(GraphNode *C) { Children.push_back(C); }

  /// Remove a specific edge
  void removeEdge(iterator I) {
    *I = Children.back();
    Children.pop_back();
  }

private:
  Value *V;

  std::vector<GraphNode *> Children;

  bool onStack;

  int discovered;

  int low;

  GraphNode(const GraphNode &) = delete;
  void operator=(const GraphNode &) = delete;
};
} // namespace uscc
} // namespace llvm

#endif // LLVM_USCC_GRAPH_H
