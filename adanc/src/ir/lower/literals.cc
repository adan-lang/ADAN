#include <llvm/IR/Constants.h>

#include "ast.hh"
#include "lower.hh"

void IRLowerer::visit(IntLiteral &node)
{
    result = llvm::ConstantInt::getSigned(builder.getInt64Ty(), node.value);
}

void IRLowerer::visit(FloatLiteral &node)
{
    result = llvm::ConstantFP::get(builder.getDoubleTy(), node.value);
}

void IRLowerer::visit(BoolLiteral &node)
{
    result = llvm::ConstantInt::get(builder.getInt1Ty(), node.value);
}

void IRLowerer::visit(StringLiteral &node)
{
    result = builder.CreateGlobalString(node.value);
}