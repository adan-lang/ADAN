#pragma once

#include <stdexcept>

#include "visitor.hh"
#include "symbol_table.hh"
#include "type.hh"

class SemanticAnalyzer : public ASTVisitor
{
    SymbolTable table;
    TypePtr last_expr_type;

public:
    void visit(IntLiteral &) override;
    void visit(FloatLiteral &) override;
    void visit(BoolLiteral &) override;
    void visit(StringLiteral &) override;
    void visit(InterpolatedString &) override;
    void visit(Identifier &expr) override;
    void visit(ArrayLiteral &expr) override;
    void visit(CallExpr &expr) override;
    void visit(BinaryExpr &expr) override;
    void visit(UnaryExpr &expr) override;
    void visit(ReturnStmt &) override;
    void visit(ExprStmt &) override;
    void visit(LocalDecl &) override;
    void visit(FuncDecl &) override;
    void visit(IfStmt &) override;
    void visit(NumericForStmt &) override;
    void visit(ForInStmt &) override;
    void visit(WhileStmt &) override;

    void visit(Block &block) override;
};