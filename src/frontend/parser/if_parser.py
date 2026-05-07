from __future__ import annotations
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .parser import Parser
from .expression_parser import ParseExpression
from frontend.ast.statements import IfStmt


def ParseIfStatement(State: Parser):
    if State.IsAtEnd() or State.Peek().Lexeme != "if":
        Token = State.Peek() if not State.IsAtEnd() else None
        Message = "Expected 'if' to start if-statement"
        if Token is None:
            raise ValueError(f"{Message} at EOF")
        raise ValueError(
            f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'"
        )
    State.Advance()

    Condition = ParseExpression(State)
    if not Condition:
        Condition = []
        while not State.IsAtEnd() and State.Peek().Lexeme != ":":
            Condition.append(State.Advance())

    if State.IsAtEnd() or State.Peek().Lexeme != ":":
        Token = State.Peek() if not State.IsAtEnd() else None
        Message = "Expected ':' after if Condition"
        if Token is None:
            raise ValueError(f"{Message} at EOF")
        raise ValueError(
            f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'"
        )
    State.Advance()

    ThenBranch = []
    while not State.IsAtEnd() and State.Peek().Lexeme not in {"else"}:
        ThenBranch.append(State.Advance())

    ElseBranch = None
    if not State.IsAtEnd() and State.Peek().Lexeme == "else":
        State.Advance()

        if not State.IsAtEnd() and State.Peek().Lexeme == "if":
            ElseBranch = [ParseIfStatement(State)]
        else:
            if State.IsAtEnd() or State.Peek().Lexeme != ":":
                Token = State.Peek() if not State.IsAtEnd() else None
                Message = "Expected ':' after 'else'"
                if Token is None:
                    raise ValueError(f"{Message} at EOF")
                raise ValueError(
                    f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'"
                )
            State.Advance()
            ElseBranch = []
            while not State.IsAtEnd():
                ElseBranch.append(State.Advance())

    Node = IfStmt()
    Node.Condition = Condition
    Node.ThenBranch = ThenBranch
    Node.ElseBranch = ElseBranch
    return Node
