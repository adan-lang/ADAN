from .tokens import NewToken, TokenType, Token
from typing import List

# basically we want to take in the source code, read everything
#   and output a token stream that's functionally identical


class Scanner:
    def __init__(self, Source: str, Line: int, Column: int, Position: int):
        self.Source = Source
        self.Line = max(1, Line)
        self.Column = max(1, Column)
        self.Position = max(0, Position - 1)


# (core) helpers first


def Consume(Lexer: Scanner) -> str:
    if Lexer.Position >= len(Lexer.Source):
        return None

    Char = Lexer.Source[Lexer.Position]
    Lexer.Position += 1

    if Char == "\n":
        Lexer.Line += 1
        Lexer.Column = 1
    else:
        Lexer.Column += 1

    return Char


def Peek(Lexer: Scanner) -> str:
    if Lexer.Position >= len(Lexer.Source):
        return None

    return Lexer.Source[Lexer.Position]


def PeekNext(Lexer: Scanner) -> str:
    if Lexer.Position + 1 >= len(Lexer.Source):
        return None

    return Lexer.Source[Lexer.Position + 1]


def Match(Lexer: Scanner, Expected: str) -> bool:
    if Lexer.Position >= len(Lexer.Source):
        return False

    if Lexer.Source[Lexer.Position] == Expected:
        Consume(Lexer)
        return True
    return False


def IsIdentStart(Char: str) -> bool:
    return Char is not None and (Char.isalpha() or Char == "_")


def IsIdentPart(Char: str) -> bool:
    return Char is not None and (Char.isalnum() or Char == "_")


def TokenFromLexeme(Lexeme: str) -> TokenType:
    Keywords = {
        "function": TokenType.TOKEN_FUNCTION,
        "if": TokenType.TOKEN_IF,
        "else": TokenType.TOKEN_ELSE,
        "return": TokenType.TOKEN_RETURN,
        "pass": TokenType.TOKEN_PASS,
        "true": TokenType.TOKEN_TRUE,
        "false": TokenType.TOKEN_FALSE,
        "none": TokenType.TOKEN_NONE,
        "nil": TokenType.TOKEN_NIL,
        "or": TokenType.TOKEN_OR,
    }
    return Keywords.get(Lexeme, TokenType.TOKEN_IDENTIFIER)


def Identifiers(
    Lexer: Scanner, StartLine: int, StartColumn: int, StartPos: int
) -> Token:
    Lexeme = ""
    while IsIdentPart(Peek(Lexer)):
        Lexeme += Consume(Lexer)

    return NewToken(TokenFromLexeme(Lexeme), StartLine, StartColumn, StartPos, Lexeme)


def Numbers(Lexer: Scanner, StartLine: int, StartColumn: int, StartPos: int) -> Token:
    Lexeme = ""

    while Peek(Lexer) is not None and Peek(Lexer).isdigit():
        Lexeme += Consume(Lexer)

    if Peek(Lexer) == "." and PeekNext(Lexer) is not None and PeekNext(Lexer).isdigit():
        Lexeme += Consume(Lexer)
        while Peek(Lexer) is not None and Peek(Lexer).isdigit():
            Lexeme += Consume(Lexer)

    return NewToken(TokenType.TOKEN_NUMBER, StartLine, StartColumn, StartPos, Lexeme)


def Strings(
    Lexer: Scanner, Quote: str, StartLine: int, StartColumn: int, StartPos: int
) -> Token:
    Lexeme = Consume(Lexer)

    while True:
        Char = Peek(Lexer)
        if Char is None:
            return NewToken(
                TokenType.TOKEN_ERROR, StartLine, StartColumn, StartPos, Lexeme
            )

        Lexeme += Consume(Lexer)
        if Char == Quote:
            TokType = (
                TokenType.TOKEN_INTERPOLATION
                if Quote == "`"
                else TokenType.TOKEN_STRING
            )
            return NewToken(TokType, StartLine, StartColumn, StartPos, Lexeme)


def SinglelineComment(Lexer: Scanner) -> None:
    Consume(Lexer)
    Consume(Lexer)
    while Peek(Lexer) is not None and Peek(Lexer) != "\n":
        Consume(Lexer)


def MultilineComment(Lexer: Scanner) -> bool:
    Consume(Lexer)
    Consume(Lexer)
    Consume(Lexer)
    Consume(Lexer)
    while True:
        Char = Peek(Lexer)
        if Char is None:
            return False
        if Char == "]" and PeekNext(Lexer) == "]":
            Consume(Lexer)
            Consume(Lexer)
            return True
        Consume(Lexer)


# extra helpers


def PassIgnored(Lexer: Scanner) -> Token:
    while True:
        Char = Peek(Lexer)
        if Char is None:
            return None

        if Char in {" ", "\t", "\r", "\n"}:
            Consume(Lexer)
            continue

        if Char == "-" and PeekNext(Lexer) == "-":
            Source = Lexer.Source
            Pos = Lexer.Position
            if Pos + 3 < len(Source) and Source[Pos : Pos + 4] == "--[[":
                if not MultilineComment(Lexer):
                    return NewToken(
                        TokenType.TOKEN_ERROR,
                        Lexer.Line,
                        Lexer.Column,
                        Lexer.Position,
                        "Unterminated multiline comment",
                    )
                continue
            SinglelineComment(Lexer)
            continue

        return None


def NextToken(Lexer: Scanner):
    IgnoredError = PassIgnored(Lexer)
    if IgnoredError is not None:
        return IgnoredError

    Char = Peek(Lexer)
    StartLine = Lexer.Line
    StartColumn = Lexer.Column
    StartPos = Lexer.Position

    if Char is None:
        return NewToken(TokenType.TOKEN_EOF, StartLine, StartColumn, StartPos, "EOF")

    if IsIdentStart(Char):
        return Identifiers(Lexer, StartLine, StartColumn, StartPos)

    if Char.isdigit():
        return Numbers(Lexer, StartLine, StartColumn, StartPos)

    if Char in {'"', "'", "`"}:
        return Strings(Lexer, Char, StartLine, StartColumn, StartPos)

    if Char == "-" and PeekNext(Lexer) == ">":
        Consume(Lexer)
        Consume(Lexer)
        return NewToken(TokenType.TOKEN_ARROW, StartLine, StartColumn, StartPos, "->")

    if Char == "(":
        Consume(Lexer)
        return NewToken(
            TokenType.TOKEN_LEFT_PAREN, StartLine, StartColumn, StartPos, "("
        )

    if Char == ")":
        Consume(Lexer)
        return NewToken(
            TokenType.TOKEN_RIGHT_PAREN, StartLine, StartColumn, StartPos, ")"
        )

    if Char == ":":
        Consume(Lexer)
        return NewToken(TokenType.TOKEN_COLON, StartLine, StartColumn, StartPos, ":")

    if Char == ",":
        Consume(Lexer)
        return NewToken(TokenType.TOKEN_COMMA, StartLine, StartColumn, StartPos, ",")

    if Char == "=":
        Consume(Lexer)
        return NewToken(TokenType.TOKEN_EQUAL, StartLine, StartColumn, StartPos, "=")

    if Char == "-":
        Consume(Lexer)
        return NewToken(TokenType.TOKEN_MINUS, StartLine, StartColumn, StartPos, "-")

    # fallback (important so scanner always advances)
    Consume(Lexer)
    return NewToken(TokenType.TOKEN_ERROR, StartLine, StartColumn, StartPos, Char)


# (core) lexical scanner


def NewScanner(Source: str, Line: int, Column: int, Position: int) -> Scanner:
    Lexer = Scanner(Source, Line, Column, Position)
    return Lexer


def Scan(Lexer: Scanner) -> List[Token]:
    Tokens = []

    while True:
        Tok = NextToken(Lexer)
        Tokens.append(Tok)

        if Tok.TokType == TokenType.TOKEN_EOF:
            break

    return Tokens
