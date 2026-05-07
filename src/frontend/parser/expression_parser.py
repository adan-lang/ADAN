from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .parser import Parser
from frontend.scanner.tokens import TokenType
from frontend.ast.expressions import (
    IdentifierExpr,
    LiteralExpr,
    CallExpr,
    InterpolationExpr,
)


def ParseExpression(State: Parser):
    if State.IsAtEnd():
        return None

    Tok = State.Peek()

    if Tok.TokType in {
        TokenType.TOKEN_NUMBER,
        TokenType.TOKEN_STRING,
        TokenType.TOKEN_TRUE,
        TokenType.TOKEN_FALSE,
        TokenType.TOKEN_NONE,
        TokenType.TOKEN_NIL,
    }:
        Node = LiteralExpr()
        Node.Value = State.Advance()
        return Node

    if Tok.TokType == TokenType.TOKEN_INTERPOLATION:
        Node = InterpolationExpr()
        Node.Value = State.Advance()
        return Node

    if Tok.TokType == TokenType.TOKEN_IDENTIFIER:
        Name = State.Advance()

        if not State.IsAtEnd() and State.Peek().TokType == TokenType.TOKEN_LEFT_PAREN:
            State.Advance()
            Arguments = []

            if (
                not State.IsAtEnd()
                and State.Peek().TokType != TokenType.TOKEN_RIGHT_PAREN
            ):
                while True:
                    Arg = ParseExpression(State)
                    if not Arg:
                        return None
                    Arguments.append(Arg)

                    if State.IsAtEnd():
                        return None

                    if State.Peek().TokType == TokenType.TOKEN_COMMA:
                        State.Advance()
                        continue

                    if State.Peek().TokType == TokenType.TOKEN_RIGHT_PAREN:
                        break

                    return None

            if State.IsAtEnd() or State.Peek().TokType != TokenType.TOKEN_RIGHT_PAREN:
                return None
            State.Advance()

            Node = CallExpr()
            Node.Callee = Name
            Node.Arguments = Arguments
            return Node

        Node = IdentifierExpr()
        Node.Name = Name
        return Node

    return None
