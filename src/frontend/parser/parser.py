from typing import List

from frontend.scanner.tokens import Token, TokenType
from .function_parser import ParseFunctionDef
from .if_parser import ParseIfStatement
from .return_parser import ParseReturnStmt
from .expression_parser import ParseExpression
from frontend.ast.statements import AssignmentStmt, ExpressionStmt, PassStmt, ReturnStmt


class Parser:
    def __init__(self, Tokens: List[Token]):
        self.Tokens = Tokens
        self.Current = 0

    def Parse(self):
        Nodes = []

        while not self.IsAtEnd():
            Node = None
            Tok = self.Peek()

            if Tok.TokType == TokenType.TOKEN_FUNCTION:
                Node = ParseFunctionDef(self)
            elif Tok.TokType == TokenType.TOKEN_IF:
                Node = ParseIfStatement(self)
            elif Tok.TokType == TokenType.TOKEN_RETURN:
                Node = ParseReturnStmt(self)
            elif Tok.TokType == TokenType.TOKEN_PASS:
                self.Advance()
                Node = PassStmt()
            elif (
                Tok.TokType == TokenType.TOKEN_IDENTIFIER
                and self.Current + 1 < len(self.Tokens)
                and self.Tokens[self.Current + 1].TokType == TokenType.TOKEN_EQUAL
            ):
                Name = self.Advance()
                self.Advance()
                Value = ParseExpression(self)
                if not Value:
                    if self.IsAtEnd():
                        raise ValueError("Expected expression after '=' at EOF")
                    Token = self.Peek()
                    raise ValueError(
                        f"Expected expression after '=' at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'"
                    )
                Node = AssignmentStmt()
                Node.Name = Name
                Node.Value = Value
            else:
                Expr = ParseExpression(self)
                if not Expr:
                    if self.IsAtEnd():
                        raise ValueError("Expected expression-statement at EOF")
                    Token = self.Peek()
                    raise ValueError(
                        f"Expected expression-statement at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'"
                    )
                Node = ExpressionStmt()
                Node.Expression = Expr

            Nodes.append(Node)

        return Nodes

    def Peek(self):
        return self.Tokens[self.Current]

    def Previous(self):
        return self.Tokens[self.Current - 1]

    def Advance(self):
        Current = self.Tokens[self.Current]
        self.Current += 1

        return Current

    def Match(self, *TokTypes):
        for Tok in TokTypes:
            if self.Peek().TokType == Tok:
                self.Advance()
                return True
        return False

    def Consume(self, TokType, Message: str):
        if self.Tokens[self.Current].TokType == TokType:
            self.Advance()
        else:
            pass  # raise parse error later on

    def IsAtEnd(self) -> bool:
        return (
            self.Current >= len(self.Tokens)
            or self.Peek().TokType == TokenType.TOKEN_EOF
        )
