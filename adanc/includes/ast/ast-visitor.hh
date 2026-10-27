#pragma once

#include <print>

#include "ast.hh"

class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;

    // @todo As you implement a func, change from {} to = 0
    virtual void visit(IntLiteral &node) = 0;
    virtual void visit(FloatLiteral &node) {};
    virtual void visit(BoolLiteral &node) {};
    virtual void visit(StringLiteral &node) {};
    virtual void visit(InterpolatedString &node) {};
    virtual void visit(Identifier &node) {};
    virtual void visit(ArrayLiteral &node) {};
    virtual void visit(ArrayIndex &node) {};
    virtual void visit(CallExpr &node) {};
    virtual void visit(BinaryExpr &node) {};
    virtual void visit(UnaryExpr &node) {};
    virtual void visit(ReturnStmt &node) {};
    virtual void visit(ExprStmt &node) {};
    virtual void visit(LocalDecl &node) = 0;
    virtual void visit(FuncDecl &node) = 0;
    virtual void visit(IfStmt &node) {};
    virtual void visit(NumericForStmt &node) {};
    virtual void visit(ForInStmt &node) {};
    virtual void visit(WhileStmt &node) {};
    virtual void visit(Block &node) {};
};