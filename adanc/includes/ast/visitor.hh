#pragma once

#include "ast.hh"

struct ASTVisitor
{
    virtual ~ASTVisitor() = default;

    virtual void visit(IntLiteral &node) = 0;
    virtual void visit(FloatLiteral &node) = 0;
    virtual void visit(BoolLiteral &node) = 0;
    virtual void visit(StringLiteral &node) = 0;
    virtual void visit(InterpolatedString &node) = 0;
    virtual void visit(Identifier &node) = 0;
    virtual void visit(ArrayLiteral &node) = 0;
    virtual void visit(CallExpr &node) = 0;
    virtual void visit(BinaryExpr &node) = 0;
    virtual void visit(UnaryExpr &node) = 0;
    virtual void visit(ReturnStmt &node) = 0;
    virtual void visit(ExprStmt &node) = 0;
    virtual void visit(LocalDecl &node) = 0;
    virtual void visit(FuncDecl &node) = 0;
    virtual void visit(IfStmt &node) = 0;
    virtual void visit(NumericForStmt &node) = 0;
    virtual void visit(ForInStmt &node) = 0;
    virtual void visit(WhileStmt &node) = 0;

    virtual void visit(Block &) = 0;
};