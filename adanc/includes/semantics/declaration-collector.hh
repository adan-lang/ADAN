#pragma once

#include "visitor.hh"

struct DeclarationCollector : ASTVisitor
{
    virtual void visit(IntLiteral &node) override;
    virtual void visit(FloatLiteral &node) override;
    virtual void visit(BoolLiteral &node) override;
    virtual void visit(StringLiteral &node) override;
    virtual void visit(InterpolatedString &node) override;
    virtual void visit(Identifier &node) override;
    virtual void visit(ArrayLiteral &node) override;
    virtual void visit(CallExpr &node) override;
    virtual void visit(BinaryExpr &node) override;
    virtual void visit(UnaryExpr &node) override;
    virtual void visit(ReturnStmt &node) override;
    virtual void visit(ExprStmt &node) override;
    virtual void visit(LocalDecl &node) override;
    virtual void visit(FuncDecl &node) override;
    virtual void visit(IfStmt &node) override;
    virtual void visit(NumericForStmt &node) override;
    virtual void visit(ForInStmt &node) override;
    virtual void visit(WhileStmt &node) override;

    virtual void visit(Block &) override;
};