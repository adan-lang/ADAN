#include "lower.hh"

void ASTLowerer::visit(ReturnStmt &node)
{
    llvm::Value *value = lower_expr(*node.value);
    if (value->getType()->isVoidTy())
        builder.CreateRetVoid();
    else
        builder.CreateRet(value);
}

void ASTLowerer::visit(ExprStmt &node)
{
    lower_expr(*node.expr);
}

void ASTLowerer::visit(IfStmt &node)
{
    llvm::Value *condition = lower_expr(*node.condition);
    
}