#include <llvm/IR/Constants.h>

#include "ast.hh"
#include "lower.hh"

void ASTLowerer::visit(IntLiteral &node)
{
    result = llvm::ConstantInt::getSigned(builder.getInt64Ty(), node.value);
}

void ASTLowerer::visit(FloatLiteral &node)
{
    result = llvm::ConstantFP::get(builder.getDoubleTy(), node.value);
}

void ASTLowerer::visit(BoolLiteral &node)
{
    result = llvm::ConstantInt::get(builder.getInt1Ty(), node.value);
}

void ASTLowerer::visit(StringLiteral &node)
{
    result = builder.CreateGlobalString(node.value);
}