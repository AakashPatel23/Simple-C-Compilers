//
//  ASTNodes.cpp
//  uscc
//
//  Implements the printNode function for every AST node
//
//---------------------------------------------------------
//  Copyright (c) 2014, Sanjay Madhav
//  All rights reserved.
//
//  This file is distributed under the BSD license.
//  See LICENSE.TXT for details.
//---------------------------------------------------------

#include "ASTNodes.h"
#include "Symbols.h"

using namespace uscc::parse;
using namespace uscc::scan;

using std::shared_ptr;

// DON'T TRY THIS AT HOME
#define AST_PRINT(a) void a::printNode(std::ostream& output, int depth) const noexcept \
{ \
for (int i = 0; i < depth; i++) \
{ \
	output << "---"; \
}

AST_PRINT(ASTProgram)
	output << "Program:" << std::endl;
	for (auto func : mFuncs)
	{
		func->printNode(output, depth + 1);
	}
}

AST_PRINT(ASTFunction)
	output << "Function: ";
	switch (mReturnType)
	{
		case Type::Void:
			output << "void ";
			break;
		case Type::Int:
			output << "int ";
			break;
		case Type::Char:
			output << "char ";
			break;
		default:
			output <<  "Shouldn't have gotten here. ";
			break;
	}
	output << mIdent.getName() << std::endl;

	for (auto arg : mArgs)
	{
		arg->printNode(output, depth + 1);
	}
	
	mBody->printNode(output, depth + 1);
}

AST_PRINT(ASTArgDecl)
	output << "ArgDecl: ";
	switch (mIdent.getType())
	{
		case Type::Void:
			output << "void ";
			break;
		case Type::Int:
			output << "int ";
			break;
		case Type::Char:
			output << "char ";
			break;
		case Type::IntArray:
			output << "int[] ";
			break;
		case Type::CharArray:
			output << "char[] ";
			break;
		default:
			output << "Shouldn't have gotten here...";
			break;
	}
	output << mIdent.getName() << std::endl;
}

AST_PRINT(ASTArraySub)
	output << "ArraySub: " << mIdent.getName() << std::endl;
	mExpr->printNode(output, depth + 1);
}

// Expressions
AST_PRINT(ASTBadExpr)
	output << "BadExpr:" <<std::endl;
}

AST_PRINT(ASTLogicalAnd)
	output << "LogicalAnd: " << std::endl;
	mLHS->printNode(output, depth + 1);
	mRHS->printNode(output, depth + 1);
}

AST_PRINT(ASTLogicalOr)
	output << "LogicalOr: " << std::endl;
	mLHS->printNode(output, depth + 1);
	mRHS->printNode(output, depth + 1);
}

AST_PRINT(ASTBinaryCmpOp)
output << "BinaryCmp " << Token::Values[mOp] << ':' << std::endl;
	mLHS->printNode(output, depth + 1);
	mRHS->printNode(output, depth + 1);
}

AST_PRINT(ASTBinaryMathOp)
	output << "BinaryMath " << Token::Values[mOp] << ':' << std::endl;
	mLHS->printNode(output, depth + 1);
	mRHS->printNode(output, depth + 1);
}

// Value -->
AST_PRINT(ASTNotExpr)
	output << "NotExpr:" << std::endl;
	mExpr->printNode(output, depth + 1);
}

// Factor -->
AST_PRINT(ASTConstantExpr)
	output << "ConstantExpr: " << mValue << std::endl;
}

AST_PRINT(ASTStringExpr)
	output << "StringExpr: " << mString->getText() << std::endl;
}

AST_PRINT(ASTIdentExpr)
	output << "IdentExpr: " << mIdent.getName() << std::endl;
}

AST_PRINT(ASTArrayExpr)
	output << "ArrayExpr: " << std::endl;
	mArray->printNode(output, depth + 1);
}

AST_PRINT(ASTFuncExpr)
output << "FuncExpr: " << mIdent.getName() << std::endl;
	for (auto arg : mArgs)
	{
		arg->printNode(output, depth + 1);
	}
}

AST_PRINT(ASTIncExpr)
	output << "IncExpr: " << mIdent.getName() << std::endl;
}

AST_PRINT(ASTDecExpr)
	output << "DecExpr: " << mIdent.getName() << std::endl;
}

AST_PRINT(ASTAddrOfArray)
	output << "AddrOfArray:" << std::endl;
	mArray->printNode(output, depth + 1);
}
			
AST_PRINT(ASTToIntExpr)
	output << "ToIntExpr: " << std::endl;
	mExpr->printNode(output, depth + 1);
}
			
AST_PRINT(ASTToCharExpr)
	output << "ToCharExpr: " << std::endl;
	mExpr->printNode(output, depth + 1);
}


// Declaration
AST_PRINT(ASTDecl)
	output << "Decl: ";
	switch (mIdent.getType())
	{
		case Type::Void:
			output << "void";
			break;
		case Type::Int:
			output << "int";
			break;
		case Type::Char:
			output << "char";
			break;
		case Type::IntArray:
			output << "int[" << mIdent.getArrayCount() << ']';
			break;
		case Type::CharArray:
			output << "char[" << mIdent.getArrayCount() << ']';
			break;
		default:
			output << "Shouldn't have gotten here...";
			break;
	}
	output << ' ' << mIdent.getName() << std::endl;
	if (mExpr)
	{
		mExpr->printNode(output, depth + 1);
	}
}

// Statements
AST_PRINT(ASTCompoundStmt)
	output << "CompoundStmt:" << std::endl;
	for (auto decl : mDecls)
	{
		decl->printNode(output, depth + 1);
	}
	for (auto stmt : mStmts)
	{
		stmt->printNode(output, depth + 1);
	}
}

AST_PRINT(ASTReturnStmt)
	if (!mExpr)
	{
		output << "ReturnStmt: (empty)" << std::endl;
	}
	else
	{
		output << "ReturnStmt:" << std::endl;
		mExpr->printNode(output, depth + 1);
	}
}

AST_PRINT(ASTAssignStmt)
	output << "AssignStmt: " << mIdent.getName() << std::endl;
	mExpr->printNode(output, depth + 1);
}

AST_PRINT(ASTAssignArrayStmt)
	output << "AssignArrayStmt:" << std::endl;
	mArray->printNode(output, depth + 1);
	mExpr->printNode(output, depth + 1);
}

AST_PRINT(ASTIfStmt)
	output << "IfStmt: " << std::endl;
	mExpr->printNode(output, depth + 1);
    for (int i = 0; i < depth+1; i++) {
        output << "---";
    }
	output << "ThenStmt: " << std::endl;
	mThenStmt->printNode(output, depth + 2);
	if (mElseStmt)
	{
        for (int i = 0; i < depth+1; i++) {
            output << "---";
        }
	    output << "ElseStmt: " << std::endl;
	    mElseStmt->printNode(output, depth + 2);
	}
}

AST_PRINT(ASTWhileStmt)
	output << "WhileStmt" << std::endl;
	mExpr->printNode(output, depth + 1);
	mLoopStmt->printNode(output, depth + 1);
}

AST_PRINT(ASTExprStmt)
	output << "ExprStmt" << std::endl;
	mExpr->printNode(output, depth + 1);
}

AST_PRINT(ASTNullStmt)
	output << "NullStmt" << std::endl;
}

AST_PRINT(ASTBreakStmt)
	output << "BreakStmt" << std::endl;
}

AST_PRINT(ASTContinueStmt)
	output << "ContinueStmt" << std::endl;
}

AST_PRINT(ASTForStmt)
	output << "ForStmt" << std::endl;
    for (int i = 0; i < depth+1; i++) {
        output << "---";
    }
    if (!minitStmt) {
        output << "initStmt (empty)" << std::endl;
    }
    else {
        output << "initStmt" << std::endl;
        minitStmt->printNode(output, depth + 2);
    }

    for (int i = 0; i < depth+1; i++) {
        output << "---";
    }
    if (!mcondExpr) {
        output << "condExpr (empty)" << std::endl;
    }
    else {
        output << "condExpr" << std::endl;
        mcondExpr->printNode(output, depth + 2);
    }

    for (int i = 0; i < depth+1; i++) {
        output << "---";
    }
    if (!mstepStmt) {
        output << "stepStmt (empty)" << std::endl;
    }
    else {
        output << "stepStmt" << std::endl;
        mstepStmt->printNode(output, depth + 2);
    }

    for (int i = 0; i < depth+1; i++) {
        output << "---";
    }
    if (!mbodyStmt) {
        output << "bodyStmt (empty)" << std::endl;
    }
    else {
        output << "bodyStmt" << std::endl;
        mbodyStmt->printNode(output, depth + 2);
    }
}

AST_PRINT(ASTDoWhileStmt)
	output << "DoWhileStmt" << std::endl;
	mExpr->printNode(output, depth + 1);
    mLoopStmt->printNode(output, depth + 1);
}

AST_PRINT(ASTSwitchStmt)
	output << "SwitchStmt" << std::endl;
	mExpr->printNode(output, depth + 1);
	for (auto stmt : case_default_stmts)
	{
        if (auto caseStmt = std::dynamic_pointer_cast<ASTCaseStmt>(stmt)) {
            caseStmt->printNode(output, depth + 1);
        }
        else if (auto defaultStmt = std::dynamic_pointer_cast<ASTDefaultStmt>(stmt)){
            defaultStmt->printNode(output, depth + 1);
        }
	}
}

AST_PRINT(ASTCaseStmt)
	output << "CaseStmt" << std::endl;
	mExpr->printNode(output, depth + 1);
	for (auto stmt : mStmts)
	{
		stmt->printNode(output, depth + 1);
	}
}

AST_PRINT(ASTDefaultStmt)
	output << "DefaultStmt" << std::endl;
	for (auto stmt : mStmts)
	{
		stmt->printNode(output, depth + 1);
	}
}
