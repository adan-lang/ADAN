from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .parser import Parser
from .expression_parser import ParseExpression
from frontend.ast.statements import ReturnStmt


def ParseReturnStmt(State: Parser):
    State.Advance()
    Expr = ParseExpression(State)
    if not Expr:
        if State.IsAtEnd():
            raise ValueError("Expected expression after 'return' at EOF")
        Token = State.Peek()
        raise ValueError(
            f"Expected expression after 'return' at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'"
        )
    Node = ReturnStmt()
    Node.Expression = Expr
    return Node
