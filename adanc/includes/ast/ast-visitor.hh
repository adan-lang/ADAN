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
    virtual void visit(ArrayLiteral &) {}

    virtual void visit(ArrayIndex &) {}
    virtual void visit(CallExpr &) {}
    virtual void visit(BinaryExpr &) {}
    virtual void visit(Identifier &) {}
    virtual void visit(UnaryExpr &) {}

    virtual void visit(ReturnStmt &) {}
    virtual void visit(ExprStmt &) {}           // <-- TODO
    virtual void visit(IfStmt &) {}             // <-- TODO
    virtual void visit(NumericForStmt &) {}     // <-- TODO
    virtual void visit(ForInStmt &) {}          // <-- TODO
    virtual void visit(WhileStmt &) {}          // <-- TODO

    virtual void visit(LocalDecl &) {}
    virtual void visit(FuncDecl &) {}

    virtual void visit(Block &) {}              // <-- TODO
};