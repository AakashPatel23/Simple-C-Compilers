
//
//  SSABuilder.cpp
//  uscc
//
//  Implements SSABuilder class
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "SSABuilder.h"
#include "../parse/Symbols.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constants.h>
#pragma clang diagnostic pop

#include <list>
#include <algorithm>

using namespace uscc::opt;
using namespace uscc::parse;
using namespace llvm;

// Called when a new function is started to clear out all the data
void SSABuilder::reset()
{
  mVarDefs.clear();
  mIncompletePhis.clear();
  mSealedBlocks.clear();
}

// For a specific variable in a specific basic block, write its value
void SSABuilder::writeVariable(Identifier* var, BasicBlock* block, Value* value)
{
  mVarDefs[block][var] = value;
}

// Read the value assigned to the variable in the requested basic block
// Will recursively search predecessor blocks if it was not written in this block
Value* SSABuilder::readVariable(Identifier* var, BasicBlock* block)
{
  return mVarDefs[block].count(var) ? mVarDefs[block][var] : readVariableRecursive(var, block);
}

// This is called to add a new block to the maps
void SSABuilder::addBlock(BasicBlock* block, bool isSealed /* = false */)
{
  mVarDefs[block];
  mIncompletePhis[block];
  if (isSealed) {
    sealBlock(block);
  }
}

// This is called when a block is "sealed" which means it will not have any
// further predecessors added. It will complete any PHI nodes (if necessary)
void SSABuilder::sealBlock(llvm::BasicBlock* block)
{
  for (auto &var : mIncompletePhis[block]) {
    addPhiOperands(var.first, var.second);
  }
  mSealedBlocks.insert(block);
}

// Recursively search predecessor blocks for a variable
Value* SSABuilder::readVariableRecursive(Identifier* var, BasicBlock* block)
{
  Value* retVal = nullptr;
  if (mSealedBlocks.find(block) == mSealedBlocks.end()) {
    PHINode* phi = PHINode::Create(var->llvmType(), 0, "Phi", block);
    if (block->getFirstNonPHI() != block->end()) {
      phi->removeFromParent();
      phi->insertBefore(&(*block->getFirstNonPHI()));
    }
    mIncompletePhis[block][var] = phi;
    retVal = phi;
  }
  else if (block->getSinglePredecessor()) {
    retVal = readVariable(var, block->getSinglePredecessor());
  }
  else {


    PHINode* phi = PHINode::Create(var->llvmType(), 0, "Phi", block);
    if (block->getFirstNonPHI() != block->end()) {
      phi->removeFromParent();
      phi->insertBefore(&(*block->getFirstNonPHI()));
    }
    writeVariable(var, block, phi);
    retVal = addPhiOperands(var, phi);
  }
  writeVariable(var, block, retVal);
  return retVal;
}

// Adds phi operands based on predecessors of the containing block
Value* SSABuilder::addPhiOperands(Identifier* var, PHINode* phi)
{
  for (auto pred = pred_begin(phi->getParent()); pred != pred_end(phi->getParent()); ++pred) {
    phi->addIncoming(readVariable(var, *pred), *pred);
  }
  return tryRemoveTrivialPhi(phi);
}

// Removes trivial phi nodes
Value* SSABuilder::tryRemoveTrivialPhi(llvm::PHINode* phi)
{
  Value* same = nullptr;

  for (int i = 0; i < phi->getNumIncomingValues(); i++) {
    auto op = phi->getIncomingValue(i);
    if (op == same || op == phi) {
      continue;
    }
    if (same) {
      return phi;
    }
    same = op;
  }

  if (!same) {
    same = UndefValue::get(phi->getType());
  }

  std::vector<User*> users;
  for (auto use = phi->user_begin(); use != phi->user_end(); ++use) {
    // user_begin()/user_end() yields the same User* once per operand slot
    // that references phi, so a user with phi as more than one incoming
    // value (common once a switch's higher fan-in merges the same value
    // through several predecessors) would otherwise appear here twice.
    // Processing it twice below would erase it, then recurse into the
    // now-dangling pointer on the second pass.
    if (*use != phi && std::find(users.begin(), users.end(), *use) == users.end()) {
      users.push_back(*use);
    }
  }

  phi->replaceAllUsesWith(same);
  auto block = phi->getParent();
  for (auto &def : mVarDefs[block]) {
    if (def.second == phi) {
      def.second = same;
    }
  }
  phi->eraseFromParent();

  for (auto &user : users) {
    if (isa<PHINode>(user)) {
      tryRemoveTrivialPhi(dyn_cast<PHINode>(user));
    }
  }

  return same;
}
