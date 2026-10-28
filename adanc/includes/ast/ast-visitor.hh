#pragma once

#include "ast.hh"

class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(IntLiteral &) {}
    virtual void visit(FloatLiteral &) {}
    virtual void visit(BoolLiteral &) {}
    virtual void visit(StringLiteral &) {}
    virtual void visit(InterpolatedString &) {}
    virtual void visit(Identifier &) {}
    virtual void visit(ArrayLiteral &) {}
    virtual void visit(ArrayIndex &) {}
    virtual void visit(CallExpr &) {}
    virtual void visit(BinaryExpr &) {}
    virtual void visit(UnaryExpr &) {}

    virtual void visit(LocalDecl &) {}
    virtual void visit(FuncDecl &) {}

    virtual void visit(ReturnStmt &) {}
    virtual void visit(ExprStmt &) {}
    virtual void visit(IfStmt &) {}
    virtual void visit(NumericForStmt &) {}
    virtual void visit(ForInStmt &) {}
    virtual void visit(WhileStmt &) {}

    virtual void visit(Block &) {}
};