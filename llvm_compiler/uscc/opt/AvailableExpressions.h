/**
 * USC Compiler
 *
 * This file implements a LLVM pass for Available Expressions Analysis.
 * It is a forward, "must" data-flow analysis.
 */

#ifndef USCC_OPT_AVAILABLEEXPRESSIONS_H
#define USCC_OPT_AVAILABLEEXPRESSIONS_H

#include "Passes.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Instructions.h"
#include <set>
#include <map>
#include <vector>

namespace llvm {
class AvailableExpressions : public FunctionPass {
private:
    // Map an expression instruction to a unique ID(starting from 0)
    std::map<Instruction*, uint32_t> expr2Id;
    // Map an ID back to the instruction
    // Index of the vector is the ID.
    // e.g. id2Expr[0] = ExprA --> ExprA's ID is 0
    std::vector<Instruction*> id2Expr;
    // "Universal" set to store all the IDs of the expressions you found
    std::set<uint32_t> universalSet;

    // bb2In=IN[BB] and bb2Out=OUT[BB] sets for each basic block
    std::map<BasicBlock*, std::set<uint32_t>> bb2In;
    std::map<BasicBlock*, std::set<uint32_t>> bb2Out;

    // GEN and KILL sets (IDs) for each basic block.
    std::map<BasicBlock*, std::set<uint32_t>> bb2Gen;
    std::map<BasicBlock*, std::set<uint32_t>> bb2Kill;
public:
    static char ID;
    AvailableExpressions();

    bool runOnFunction(llvm::Function &F) override;

    void releaseMemory() override {
        expr2Id.clear();
        id2Expr.clear();
        bb2In.clear();
        bb2Out.clear();
    }

    /**
     * Client query function. Checks if a given expression is available
     * immediately AFTER a specific instruction.
     * @param expr The instruction representing the expression to check.
     * @param point The instruction after which to check for availability.
     * @return true if the expression is available.
     */
    bool isAvailableAfter(Instruction &expr, Instruction &point);

    void dumpAvailableExpressions(llvm::Function &F, unsigned iterationCount);

    std::vector<Instruction*> getInSet(BasicBlock* bb);
};
}

#endif //USCC_OPT_AVAILABLEEXPRESSIONS_H
