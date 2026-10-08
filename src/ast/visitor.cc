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