#include "semantic.hh"

void SemanticAnalyzer::visit(IntLiteral &)
{
    last_expr_type = "int";
}

void SemanticAnalyzer::visit(FloatLiteral &)
{
}

void SemanticAnalyzer::visit(BoolLiteral &)
{
}

void SemanticAnalyzer::visit(StringLiteral &)
{
}

void SemanticAnalyzer::visit(InterpolatedString &)
{
}

void SemanticAnalyzer::visit(Identifier &)
{
}

void SemanticAnalyzer::visit(ArrayLiteral &)
{
}

void SemanticAnalyzer::visit(CallExpr &)
{
}

void SemanticAnalyzer::visit(BinaryExpr &)
{
}

void SemanticAnalyzer::visit(UnaryExpr &)
{
}

void SemanticAnalyzer::visit(ReturnStmt &)
{
}

void SemanticAnalyzer::visit(ExprStmt &)
{
}

void SemanticAnalyzer::visit(LocalDecl &)
{
}

void SemanticAnalyzer::visit(FuncDecl &)
{
}

void SemanticAnalyzer::visit(IfStmt &)
{
}

void SemanticAnalyzer::visit(NumericForStmt &)
{
}

void SemanticAnalyzer::visit(ForInStmt &)
{
}

void SemanticAnalyzer::visit(WhileStmt &)
{
}

void SemanticAnalyzer::visit(Block &block)
{
    table.enter_scope();
    for (auto &stmt : block.stmts)
        stmt->accept(*this);
    table.exit_scope();
}