#pragma once

#include <stdexcept>

#include "visitor.hh"
#include "symbol_table.hh"

class SemanticAnalyzer : public ASTVisitor
{
    SymbolTable table;
    std::string last_expr_type;

public:
    void visit(IntLiteral &) override;
    void visit(FloatLiteral &) override;
    void visit(BoolLiteral &) override;
    void visit(StringLiteral &) override;
    void visit(InterpolatedString &) override;
    void visit(Identifier &) override;
    void visit(ArrayLiteral &) override;
    void visit(CallExpr &) override;
    void visit(BinaryExpr &) override;
    void visit(UnaryExpr &) override;
    void visit(ReturnStmt &) override;
    void visit(ExprStmt &) override;
    void visit(LocalDecl &) override;
    void visit(FuncDecl &) override;
    void visit(IfStmt &) override;
    void visit(NumericForStmt &) override;
    void visit(ForInStmt &) override;
    void visit(WhileStmt &) override;

    void visit(Block &) override;
};