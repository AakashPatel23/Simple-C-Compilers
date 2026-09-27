//
//  ASTNodes.cpp
//  uscc
//
//  Implements the emitIR function for every AST node.
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "ASTNodes.h"
#include "Emitter.h"

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
#include <llvm/IR/Intrinsics.h>
#pragma clang diagnostic pop

#include <vector>

using namespace uscc::parse;
using namespace llvm;
using namespace std;

#define AST_EMIT(a) llvm::Value* a::emitIR(CodeContext& ctx) noexcept

// Program/Functions
AST_EMIT(ASTProgram)
{
  ctx.mModule = new Module("main", ctx.mGlobal);

  // Write the global string table
  ctx.mStrings.emitIR(ctx);

  // Emit declaration for stdlib "printf", if we need it
  if (ctx.mPrintfIdent != nullptr)
  {
    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(llvm::Type::getInt8PtrTy(ctx.mGlobal));

    FunctionType* printfType = FunctionType::get(llvm::Type::getInt32Ty(ctx.mGlobal),
        printfArgs, true);

    Function* func = Function::Create(printfType, GlobalValue::LinkageTypes::ExternalLinkage,
        "printf", ctx.mModule);
    func->setCallingConv(CallingConv::C);

    // Map the printf ident to this function
    ctx.mPrintfIdent->setAddress(func);
  }

  // Emit code for all the functions
  for (auto f : mFuncs)
  {
    f->emitIR(ctx);
  }
  // A program actually doesn't have a value to return, since everything
  // is stored in Module
  return nullptr;
}

AST_EMIT(ASTFunction)
{
  FunctionType* funcType = nullptr;

  // First get the return type (there's only three choices)
  llvm::Type* retType = nullptr;
  if (mReturnType == Type::Int)
  {
    retType = llvm::Type::getInt32Ty(ctx.mGlobal);
  }
  else if (mReturnType == Type::Char)
  {
    retType = llvm::Type::getInt8Ty(ctx.mGlobal);
  }
  else
  {
    retType = llvm::Type::getVoidTy(ctx.mGlobal);
  }

  if (mArgs.size() == 0)
  {
    funcType = FunctionType::get(retType, false);
  }
  else
  {

    std::vector<llvm::Type*> args;
    for (auto arg : mArgs)
    {
      args.push_back(arg->getIdent().llvmType());
    }

    funcType = FunctionType::get(retType, args, false);
  }

  // Create the function, and make it the current one
  ctx.mFunc = Function::Create(funcType,
      GlobalValue::LinkageTypes::ExternalLinkage,
      mIdent.getName(), ctx.mModule);

  // Now that we have a new function, reset our SSA builder
  ctx.mSSA.reset();

  // Map the ident to this function
  mIdent.setAddress(ctx.mFunc);

  // Create the entry basic block
  ctx.mBlock = BasicBlock::Create(ctx.mGlobal, "entry", ctx.mFunc);
  // Add and seal this block
  ctx.mSSA.addBlock(ctx.mBlock, true);

  // If we have arguments, we need to set the name/value of them
  if (mArgs.size() > 0)
  {
    Function::arg_iterator iter = ctx.mFunc->arg_begin();
    Function::arg_iterator end = ctx.mFunc->arg_end();
    int i = 0;
    while (iter != end)
    {
      Identifier& argIdent = mArgs[i]->getIdent();
      iter->setName(argIdent.getName());

      // (Technically, iter actually has the value of the
      // arg, not its address...but we will use the address
      // member for this value)
      argIdent.writeTo(ctx, iter);

      ++i;
      ++iter;
    }
  }

  ctx.mFunc->setCallingConv(CallingConv::C);

  // Add all the declarations for variables created in this function
  mScopeTable.emitIR(ctx);

  // Now emit the body
  mBody->emitIR(ctx);

  return ctx.mFunc;
}

AST_EMIT(ASTArgDecl)
{
  // This node actually doesn't have anything to emit
  return nullptr;
}

AST_EMIT(ASTArraySub)
{
  // Evaluate the sub expression to get the desired index
  Value* arrayIdx = mExpr->emitIR(ctx);

  // This address should already be saved
  Value* addr = mIdent.readFrom(ctx);

  // GEP from the array address
  IRBuilder<> build(ctx.mBlock);
  return build.CreateInBoundsGEP(addr, arrayIdx);
}

// Expressions

AST_EMIT(ASTBadExpr)
{
  // This node will never be emitted
  return nullptr;
}

AST_EMIT(ASTLogicalAnd)
{
  // This is extremely similar to logical or

  // Create the block for the RHS
  BasicBlock* rhsBlock = BasicBlock::Create(ctx.mGlobal, "and.rhs", ctx.mFunc);
  // Add the rhs block to SSA (not sealed)
  ctx.mSSA.addBlock(rhsBlock);

  // In both "true" and "false" condition, we'll jump to and.end
  // This is because we'll insert a phi node that assume false
  // if the and.end jump was from the lhs block
  BasicBlock* endBlock = BasicBlock::Create(ctx.mGlobal, "and.end", ctx.mFunc);
  // Also not sealed
  ctx.mSSA.addBlock(endBlock);

  // Now generate the LHS
  Value* lhsVal = mLHS->emitIR(ctx);

  BasicBlock* lhsBlock = ctx.mBlock;

  // Add the branch to the end of the LHS
  {
    IRBuilder<> build(ctx.mBlock);
    // We can assume it WILL be an i32 here
    // since it'd have been zero-extended otherwise
    lhsVal = build.CreateICmpNE(lhsVal, ctx.mZero, "tobool");
    build.CreateCondBr(lhsVal, rhsBlock, endBlock);
  }

  // rhsBlock should now be sealed
  ctx.mSSA.sealBlock(rhsBlock);

  // Code should now be generated in the RHS block
  ctx.mBlock = rhsBlock;
  Value* rhsVal = mRHS->emitIR(ctx);

  // This is the final RHS block (for the phi node)
  rhsBlock = ctx.mBlock;

  // Add the branch and the end of the RHS
  {
    IRBuilder<> build(ctx.mBlock);
    rhsVal = build.CreateICmpNE(rhsVal, ctx.mZero, "tobool");

    // We do an unconditional branch because the phi mode will handle
    // the correct value
    build.CreateBr(endBlock);
  }

  // endBlock should now be sealed
  ctx.mSSA.sealBlock(endBlock);

  ctx.mBlock = endBlock;

  IRBuilder<> build(ctx.mBlock);

  // Figure out the value to zext
  Value* zextVal = nullptr;

  // If rhs is not also false, we need to make a phi
  if (rhsVal != ConstantInt::getFalse(ctx.mGlobal))
  {
    PHINode* phi = build.CreatePHI(llvm::Type::getInt1Ty(ctx.mGlobal), 2);
    // If we came from the lhs, it had to be false
    phi->addIncoming(ConstantInt::getFalse(ctx.mGlobal), lhsBlock);
    phi->addIncoming(rhsVal, rhsBlock);
    zextVal = phi;
  }
  else
  {
    zextVal = ConstantInt::getFalse(ctx.mGlobal);
  }

  return build.CreateZExt(zextVal, llvm::Type::getInt32Ty(ctx.mGlobal));
}

AST_EMIT(ASTLogicalOr)
{
  // Create the block for the RHS
  BasicBlock* rhsBlock = BasicBlock::Create(ctx.mGlobal, "lor.rhs", ctx.mFunc);
  // Add the rhs block to SSA (not sealed)
  ctx.mSSA.addBlock(rhsBlock);

  // In both "true" and "false" condition, we'll jump to lor.end
  // This is because we'll insert a phi node that assume true
  // if the lor.end jump was from the lhs block
  BasicBlock* endBlock = BasicBlock::Create(ctx.mGlobal, "lor.end", ctx.mFunc);
  // Also not sealed
  ctx.mSSA.addBlock(endBlock);

  // Now generate the LHS
  Value* lhsVal = mLHS->emitIR(ctx);

  BasicBlock* lhsBlock = ctx.mBlock;

  // Add the branch to the end of the LHS
  {
    IRBuilder<> build(ctx.mBlock);
    // We can assume it WILL be an i32 here
    // since it'd have been zero-extended otherwise
    lhsVal = build.CreateICmpNE(lhsVal, ctx.mZero, "tobool");
    build.CreateCondBr(lhsVal, endBlock, rhsBlock);
  }

  // rhsBlock should now be sealed
  ctx.mSSA.sealBlock(rhsBlock);

  // Code should now be generated in the RHS block
  ctx.mBlock = rhsBlock;
  Value* rhsVal = mRHS->emitIR(ctx);

  // This is the final RHS block (for the phi node)
  rhsBlock = ctx.mBlock;

  // Add the branch and the end of the RHS
  {
    IRBuilder<> build(ctx.mBlock);
    rhsVal = build.CreateICmpNE(rhsVal, ctx.mZero, "tobool");

    // We do an unconditional branch because the phi mode will handle
    // the correct value
    build.CreateBr(endBlock);
  }

  // endBlock should now be sealed
  ctx.mSSA.sealBlock(endBlock);

  ctx.mBlock = endBlock;

  IRBuilder<> build(ctx.mBlock);

  // Figure out the value to zext
  Value* zextVal = nullptr;

  // If rhs is not also true, we need to make a phi
  if (rhsVal != ConstantInt::getTrue(ctx.mGlobal))
  {
    PHINode* phi = build.CreatePHI(llvm::Type::getInt1Ty(ctx.mGlobal), 2);
    // If we came from the lhs, it had to be false
    phi->addIncoming(ConstantInt::getTrue(ctx.mGlobal), lhsBlock);
    phi->addIncoming(rhsVal, rhsBlock);
    zextVal = phi;
  }
  else
  {
    zextVal = ConstantInt::getTrue(ctx.mGlobal);
  }

  return build.CreateZExt(zextVal, llvm::Type::getInt32Ty(ctx.mGlobal));
}

AST_EMIT(ASTBinaryCmpOp)
{
  Value *retVal = nullptr;
  Value *lhs = mLHS->emitIR(ctx);
  Value *rhs = mRHS->emitIR(ctx);
  IRBuilder<> build(ctx.mBlock);
  switch (mOp) {
    case scan::Token::EqualTo:
      retVal = build.CreateICmpEQ(lhs, rhs, "eq");
      break;
    case scan::Token::NotEqual:
      retVal = build.CreateICmpNE(lhs, rhs, "ne");
      break;
    case scan::Token::LessThan:
      retVal = build.CreateICmpSLT(lhs, rhs, "lt");
      break;
    case scan::Token::GreaterThan:
      retVal = build.CreateICmpSGT(lhs, rhs, "gt");
      break;
  }
  return build.CreateZExt(retVal, llvm::Type::getInt32Ty(ctx.mGlobal), "zext");
}

AST_EMIT(ASTBinaryMathOp)
{
  Value *lhs = mLHS->emitIR(ctx);
  Value *rhs = mRHS->emitIR(ctx);
  IRBuilder<> build(ctx.mBlock);
  switch (mOp) {
    case scan::Token::Plus:
      return build.CreateAdd(lhs, rhs, "add");
      break;
    case scan::Token::Minus:
      return build.CreateSub(lhs, rhs, "sub");
      break;
    case scan::Token::Mult:
      return build.CreateMul(lhs, rhs, "mul");
      break;
    case scan::Token::Div:
      return build.CreateSDiv(lhs, rhs, "sdiv");
      break;
    default:
      return build.CreateSRem(lhs, rhs, "srem");
  }
}

// Value -->
AST_EMIT(ASTNotExpr)
{
  IRBuilder<> build(ctx.mBlock);
  Value *retVal = build.CreateICmpEQ(mExpr->emitIR(ctx), ctx.mZero, "not");
  return build.CreateZExt(retVal, llvm::Type::getInt32Ty(ctx.mGlobal), "zext");
}

// Factor -->
AST_EMIT(ASTConstantExpr)
{
  return (mType == Type::Int) ?
    (ConstantInt::get(llvm::Type::getInt32Ty(ctx.mGlobal), mValue)) :
    (ConstantInt::get(llvm::Type::getInt8Ty(ctx.mGlobal), mValue));
}

AST_EMIT(ASTStringExpr)
{
  return mString->getValue();
}

AST_EMIT(ASTIdentExpr)
{
  return mIdent.readFrom(ctx);
}

AST_EMIT(ASTArrayExpr)
{
  // Generate the array subscript, which'll give us the address
  Value* addr = mArray->emitIR(ctx);

  IRBuilder<> build(ctx.mBlock);
  // Now load this value and return

  // NOTE: This still needs to be a load because arrays are in memory
  return build.CreateLoad(addr);
}

AST_EMIT(ASTFuncExpr)
{

  // At this point, we can assume the argument types match
  // Create the list of arguments
  std::vector<Value*> callList;
  for (auto arg : mArgs)
  {
    Value* argValue = arg->emitIR(ctx);
    // If this is an array or ptr, we need to change this to a getelemptr
    // (Provided it already isn't one)
    if (!isa<GetElementPtrInst>(argValue) &&
        argValue->getType()->isPointerTy())
    {
      if (argValue->getType()->getPointerElementType()->isArrayTy())
      {
        IRBuilder<> build(ctx.mBlock);
        std::vector<llvm::Value*> gepIdx;
        gepIdx.push_back(ctx.mZero);
        gepIdx.push_back(ctx.mZero);

        argValue = build.CreateInBoundsGEP(argValue, gepIdx);
      }
      else
      {
        IRBuilder<> build(ctx.mBlock);
        // Need to return the address of the specific index in question
        // So need a GEP
        argValue = build.CreateInBoundsGEP(argValue, ctx.mZero);
      }
    }

    callList.push_back(argValue);
  }

  // Now call the function, and return it
  Value* retVal = nullptr;

  IRBuilder<> build(ctx.mBlock);
  if (mType != Type::Void)
  {
    retVal = build.CreateCall(mIdent.getAddress(), callList, "call");
  }
  else
  {
    retVal = build.CreateCall(mIdent.getAddress(), callList);
  }

  return retVal;
}

AST_EMIT(ASTIncExpr)
{
  Value* retVal = mIdent.readFrom(ctx);
  IRBuilder<> build(ctx.mBlock);
  retVal = build.CreateAdd(retVal, ConstantInt::get(mIdent.llvmType(), 1), "inc");
  mIdent.writeTo(ctx, retVal);
  return retVal;
}

AST_EMIT(ASTDecExpr)
{
  Value* retVal = mIdent.readFrom(ctx);
  IRBuilder<> build(ctx.mBlock);
  retVal = build.CreateSub(retVal, ConstantInt::get(mIdent.llvmType(), 1), "dec");
  mIdent.writeTo(ctx, retVal);
  return retVal;
}

AST_EMIT(ASTAddrOfArray)
{
  return mArray->emitIR(ctx);
}

AST_EMIT(ASTToIntExpr)
{
  Value* exprVal = mExpr->emitIR(ctx);
  IRBuilder<> build(ctx.mBlock);
  return build.CreateSExt(exprVal, llvm::Type::getInt32Ty(ctx.mGlobal), "conv");
}

AST_EMIT(ASTToCharExpr)
{
  Value* exprVal = mExpr->emitIR(ctx);
  IRBuilder<> build(ctx.mBlock);
  return build.CreateTrunc(exprVal, llvm::Type::getInt8Ty(ctx.mGlobal), "conv");
}

// Declaration
AST_EMIT(ASTDecl)
{
  // If there's an expression, emit this also and store it in the ident
  if (mExpr)
  {
    Value* declExpr = mExpr->emitIR(ctx);

    IRBuilder<> build(ctx.mBlock);
    // If this is a string, we have to memcpy
    if (declExpr->getType()->isPointerTy())
    {
      // This address should already be saved
      Value* arrayLoc = mIdent.readFrom(ctx);

      // GEP the address of the src
      std::vector<llvm::Value*> gepIdx;
      gepIdx.push_back(ctx.mZero);
      gepIdx.push_back(ctx.mZero);

      Value*  src = build.CreateGEP(declExpr, gepIdx);

      // Memcpy into the array
      // memcpy(dest, src, size, align, volatile)
      build.CreateMemCpy(arrayLoc, src, mIdent.getArrayCount(), 1);
    }
    else
    {
      // Basic types can just be written
      mIdent.writeTo(ctx, declExpr);
    }
  }

  return nullptr;
}

// Statements
AST_EMIT(ASTCompoundStmt)
{
  for (auto decl : mDecls) {
    decl->emitIR(ctx);
  }
  for (auto stmt : mStmts) {
    if (ctx.mBlock->getTerminator()) {
      break;
    }
    stmt->emitIR(ctx);
  }
  return nullptr;
}

AST_EMIT(ASTAssignStmt)
{
  Value *value = mExpr->emitIR(ctx);
  mIdent.writeTo(ctx, value);
  return nullptr;
}

AST_EMIT(ASTAssignArrayStmt)
{
  // Generate the expression
  Value* exprVal = mExpr->emitIR(ctx);

  // Generate the array subscript, which'll give us the address
  Value* addr = mArray->emitIR(ctx);

  IRBuilder<> build(ctx.mBlock);

  // NOTE: This is still a create store because arrays are always stack-allocated
  build.CreateStore(exprVal, addr);

  return nullptr;
}

AST_EMIT(ASTIfStmt)
{
  if (mElseStmt) {
    auto then = BasicBlock::Create(ctx.mGlobal, "if.then", ctx.mFunc);
    auto el = BasicBlock::Create(ctx.mGlobal, "if.else", ctx.mFunc);

    auto end = BasicBlock::Create(ctx.mGlobal, "if.end", ctx.mFunc);
    auto expr = mExpr->emitIR(ctx);
    IRBuilder<> predBuild(ctx.mBlock);
    if (!expr->getType()->isIntegerTy(1)) {
      expr = predBuild.CreateICmpNE(expr, ctx.mZero);
    }
    predBuild.CreateCondBr(expr, then, el);

    ctx.mSSA.addBlock(then, true);
    ctx.mBlock = then;
    mThenStmt->emitIR(ctx);
    if (!ctx.mBlock->getTerminator()) {
      IRBuilder<> bodyBuild(ctx.mBlock);
      bodyBuild.CreateBr(end);
    }

    ctx.mSSA.addBlock(el, true);
    ctx.mBlock = el;
    mElseStmt->emitIR(ctx);
    if (!ctx.mBlock->getTerminator()) {
      IRBuilder<> elseBuild(ctx.mBlock);
      elseBuild.CreateBr(end);
    }

    ctx.mSSA.addBlock(end, true);
    ctx.mBlock = end;
  }
  else {
    auto then = BasicBlock::Create(ctx.mGlobal, "if.then", ctx.mFunc);
    auto end = BasicBlock::Create(ctx.mGlobal, "if.end", ctx.mFunc);

    auto expr = mExpr->emitIR(ctx);
    IRBuilder<> predBuild(ctx.mBlock);
    if (!expr->getType()->isIntegerTy(1)) {
      expr = predBuild.CreateICmpNE(expr, ctx.mZero);
    }
    predBuild.CreateCondBr(expr, then, end);

    ctx.mSSA.addBlock(then, true);
    ctx.mBlock = then;
    mThenStmt->emitIR(ctx);
    if (!ctx.mBlock->getTerminator()) {
      IRBuilder<> thenBuild(ctx.mBlock);
      thenBuild.CreateBr(end);
    }
    ctx.mSSA.addBlock(end, true);
    ctx.mBlock = end;
  }
  return nullptr;
}

AST_EMIT(ASTWhileStmt)
{
  auto condBlock = BasicBlock::Create(ctx.mGlobal, "while.cond", ctx.mFunc);

  ctx.mSSA.addBlock(condBlock);
  IRBuilder<> builder(ctx.mBlock);
  builder.CreateBr(condBlock); // unconditional branch in predecessor

  ctx.mBlock = condBlock;
  auto value = this->mExpr->emitIR(ctx);
  IRBuilder<> builderCond(ctx.mBlock);
  if (!value->getType()->isIntegerTy(1))
    value = builderCond.CreateICmpNE(value, ctx.mZero);

  auto body = BasicBlock::Create(ctx.mGlobal, "while.body", ctx.mFunc); // after expr's emitIR
  auto endBlock = BasicBlock::Create(ctx.mGlobal, "while.end", ctx.mFunc);
  builderCond.CreateCondBr(value, body, endBlock); // conditional branch in while.cond

  ctx.mSSA.addBlock(body, true);
  ctx.mBlock = body;

  // Push the continue/break targets
  ctx.mContinueBlocks.push(condBlock);
  ctx.mBreakBlocks.push(endBlock);

  this->mLoopStmt->emitIR(ctx);

  // Pop the continue/break targets
  ctx.mContinueBlocks.pop();
  ctx.mBreakBlocks.pop();

  IRBuilder<> builderBody(ctx.mBlock);
  builderBody.CreateBr(condBlock);

  ctx.mSSA.sealBlock(condBlock);

  ctx.mSSA.addBlock(endBlock, true);
  ctx.mBlock = endBlock;

  return nullptr;
}

AST_EMIT(ASTReturnStmt)
{
  IRBuilder<> build(ctx.mBlock);
  if (mExpr) {
    Value *expr = mExpr->emitIR(ctx);
    build.CreateRet(expr);
  }
  else {
    build.CreateRetVoid();
  }
  return nullptr;
}

AST_EMIT(ASTExprStmt)
{
  // Emit the expression, just return the value
  return mExpr->emitIR(ctx);
}

AST_EMIT(ASTNullStmt)
{
  // Doesn't do anything (hence empty)
  return nullptr;
}

AST_EMIT(ASTBreakStmt)
{
  IRBuilder<> build(ctx.mBlock);
  build.CreateBr(ctx.mBreakBlocks.top());
  return nullptr;
}

AST_EMIT(ASTContinueStmt)
{
  IRBuilder<> build(ctx.mBlock);
  build.CreateBr(ctx.mContinueBlocks.top());
  return nullptr;
}

AST_EMIT(ASTForStmt)
{
  auto cond = BasicBlock::Create(ctx.mGlobal, "for.cond", ctx.mFunc);

  minitStmt->emitIR(ctx);
  ctx.mSSA.addBlock(cond);
  IRBuilder<> predBuild(ctx.mBlock);
  predBuild.CreateBr(cond);

  ctx.mBlock = cond;
  auto expr = mcondExpr->emitIR(ctx);
  IRBuilder<> condBuild(ctx.mBlock);
  if (!expr->getType()->isIntegerTy(1)) {
    expr = condBuild.CreateICmpNE(expr, ctx.mZero);
  }

  auto body = BasicBlock::Create(ctx.mGlobal, "for.body", ctx.mFunc);
  auto step = BasicBlock::Create(ctx.mGlobal, "for.step", ctx.mFunc);
  auto end = BasicBlock::Create(ctx.mGlobal, "for.end", ctx.mFunc);
  condBuild.CreateCondBr(expr, body, end);

  ctx.mSSA.addBlock(body, true);
  // step and end are continue/break targets: a continue/break anywhere inside
  // mbodyStmt can add a predecessor edge to either, so both stay unsealed
  // until the whole body has been emitted.
  ctx.mSSA.addBlock(step);
  ctx.mSSA.addBlock(end);
  ctx.mBlock = body;

  ctx.mContinueBlocks.push(step);
  ctx.mBreakBlocks.push(end);

  mbodyStmt->emitIR(ctx);

  ctx.mContinueBlocks.pop();
  ctx.mBreakBlocks.pop();

  IRBuilder<> bodyBuild(ctx.mBlock);
  bodyBuild.CreateBr(step);
  ctx.mSSA.sealBlock(step);
  ctx.mBlock = step;

  mstepStmt->emitIR(ctx);
  IRBuilder<> stepBuild(ctx.mBlock);
  stepBuild.CreateBr(cond);
  ctx.mSSA.sealBlock(cond);

  ctx.mSSA.sealBlock(end);
  ctx.mBlock = end;

  return nullptr;
}

AST_EMIT(ASTDoWhileStmt)
{
  auto body = BasicBlock::Create(ctx.mGlobal, "dowhile.body", ctx.mFunc);
  auto cond = BasicBlock::Create(ctx.mGlobal, "dowhile.cond", ctx.mFunc);
  auto end = BasicBlock::Create(ctx.mGlobal, "dowhile.end", ctx.mFunc);

  IRBuilder<> predBuild(ctx.mBlock);
  predBuild.CreateBr(body);

  // body gains a second predecessor later (cond's back-edge); cond is the
  // continue target, so it can also gain predecessors from anywhere inside
  // mLoopStmt. Both stay unsealed until their full predecessor sets exist.
  ctx.mSSA.addBlock(body);
  ctx.mSSA.addBlock(cond);
  ctx.mBlock = body;

  ctx.mContinueBlocks.push(cond);
  ctx.mBreakBlocks.push(end);

  mLoopStmt->emitIR(ctx);

  ctx.mContinueBlocks.pop();
  ctx.mBreakBlocks.pop();

  IRBuilder<> bodyBuild(ctx.mBlock);

  bodyBuild.CreateBr(cond);
  ctx.mSSA.sealBlock(cond);

  ctx.mBlock = cond;
  auto expr = mExpr->emitIR(ctx);
  IRBuilder<> condBuild(ctx.mBlock);
  if (!expr->getType()->isIntegerTy(1)) {
    expr = condBuild.CreateICmpNE(expr, ctx.mZero);
  }

  condBuild.CreateCondBr(expr, body, end);
  ctx.mSSA.sealBlock(body);
  ctx.mSSA.addBlock(end, true);
  ctx.mBlock = end;

  return nullptr;
}

AST_EMIT(ASTSwitchStmt)
{
  auto end = BasicBlock::Create(ctx.mGlobal, "switch.end", ctx.mFunc);
  // end is the break target for every case body, so a break anywhere inside
  // the switch can add a predecessor edge to it -- stays unsealed until the
  // whole case list has been emitted.
  ctx.mSSA.addBlock(end);

  llvm::BasicBlock* defCase = nullptr;
  std::map<ASTCaseDefaultStmt*, llvm::BasicBlock*> caseMap;
  for (auto caseStmt : case_default_stmts) {
    if (std::dynamic_pointer_cast<ASTDefaultStmt>(caseStmt)) {
      defCase = BasicBlock::Create(ctx.mGlobal, "switch.default", ctx.mFunc);
    }
    else {
      caseMap[caseStmt.get()] = BasicBlock::Create(ctx.mGlobal, "switch.case", ctx.mFunc);
    }
  }

  if(!defCase) {
    defCase = end;
  }

  auto expr = mExpr->emitIR(ctx);
  IRBuilder<> switchBuild(ctx.mBlock);
  auto switchInst = switchBuild.CreateSwitch(expr, defCase, case_default_stmts.size());

  ctx.mBreakBlocks.push(end);
  for (auto it = case_default_stmts.begin(); it != case_default_stmts.end(); ++it) {
    auto caseStmt = *it;
    llvm::BasicBlock *caseBlock = nullptr;
    if (std::dynamic_pointer_cast<ASTDefaultStmt>(caseStmt)) {
      caseBlock = defCase;
    }
    else {
      caseBlock = caseMap.find(caseStmt.get())->second;
    }

    ctx.mBlock = caseBlock;

    if (!std::dynamic_pointer_cast<ASTDefaultStmt>(caseStmt)) {
      // getExpr() returns the case's stored "switch_var == caseValue"
      // comparison node (built by parseCaseStmt for AST printing), not the
      // raw case value -- evaluating it directly yields an icmp instruction,
      // never a ConstantInt, so dyn_cast<ConstantInt> on it is always null.
      // The switch instruction needs the actual case constant, which is the
      // comparison's RHS.
      auto cmpExpr = dynamic_pointer_cast<ASTBinaryCmpOp>(dynamic_pointer_cast<ASTCaseStmt>(caseStmt)->getExpr());
      auto caseExpr = cmpExpr->getRHS()->emitIR(ctx);
      switchInst->addCase(dyn_cast<llvm::ConstantInt>(caseExpr), caseBlock);
    }

    // caseBlock's full predecessor set exists by this point: the switch's
    // own dispatch edge (just added above for a case, or wired via
    // CreateSwitch before this loop for defCase) plus any fallthrough edge
    // from the previous case, already created in the prior iteration if
    // present. Safe to seal now.
    ctx.mSSA.addBlock(caseBlock, true);

    caseStmt->emitIR(ctx);
    if (!ctx.mBlock->getTerminator()) {

      llvm::BasicBlock* nextBlock = end;
      auto next = std::next(it);
      if (next != case_default_stmts.end()) {
        nextBlock = std::dynamic_pointer_cast<ASTDefaultStmt>(*next)? defCase : caseMap.find((*next).get())->second;
      }
      // Built from ctx.mBlock *now*, not the block this case started in --
      // caseStmt->emitIR(ctx) may have reassigned ctx.mBlock (e.g. a nested
      // if inside the case body), and the fallthrough branch must land in
      // whichever block is actually still unterminated.
      IRBuilder<> caseBuild(ctx.mBlock);
      caseBuild.CreateBr(nextBlock);
    }
  }
  ctx.mBreakBlocks.pop();
  ctx.mSSA.sealBlock(end);
  ctx.mBlock = end;

  return nullptr;
}

AST_EMIT(ASTCaseStmt)
{
  // This function is now responsible for emitting all statements within the default block.
  for (const auto& innerStmt : mStmts)
  {
    // If a terminator (like break or return) is hit, stop emitting.
    if (ctx.mBlock->getTerminator()) {
      break;
    }
    innerStmt->emitIR(ctx);
  }
  return nullptr;
}

AST_EMIT(ASTDefaultStmt)
{
  // This function is now responsible for emitting all statements within the default block.
  for (const auto& innerStmt : mStmts)
  {
    // If a terminator (like break or return) is hit, stop emitting.
    if (ctx.mBlock->getTerminator()) {
      break;
    }
    innerStmt->emitIR(ctx);
  }
  return nullptr;
}
