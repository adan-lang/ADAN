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
    void visit(ReturnStmt &stmt) override;
    void visit(ExprStmt &stmt) override;
    void visit(LocalDecl &stmt) override;
    void visit(FuncDecl &stmt) override;
    void visit(IfStmt &stmt) override;
    void visit(NumericForStmt &stmt) override;
    void visit(ForInStmt &stmt) override;
    void visit(WhileStmt &stmt) override;

    void visit(Block &block) override;
};