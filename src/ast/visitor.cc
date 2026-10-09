#include "visitor.hh"

void IntLiteral::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void FloatLiteral::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void BoolLiteral::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void StringLiteral::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void InterpolatedString::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void Identifier::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void ArrayLiteral::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void CallExpr::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void BinaryExpr::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void UnaryExpr::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void ReturnStmt::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void ExprStmt::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void LocalDecl::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void FuncDecl::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void IfStmt::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void NumericForStmt::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void ForInStmt::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}

void WhileStmt::accept(ASTVisitor &visitor)
{
    visitor.visit(*this);
}