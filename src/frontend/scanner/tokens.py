from enum import Enum, auto

# in C/C++ we're able to just do
"""
typedef enum {
    TOKEN_<TOKEN-TYPE>
} TokenType;
"""


class TokenType(Enum):
    # SPECIAL TOKENS
    TOKEN_EOF = auto()
    TOKEN_ERROR = auto()

    # LITERALS + IDENTIFIERS
    TOKEN_IDENTIFIER = auto()
    TOKEN_NUMBER = auto()
    TOKEN_STRING = auto()
    TOKEN_INTERPOLATION = auto()

    # KEYWORDS
    TOKEN_FUNCTION = auto()
    TOKEN_IF = auto()
    TOKEN_ELSE = auto()
    TOKEN_RETURN = auto()
    TOKEN_PASS = auto()
    TOKEN_TRUE = auto()
    TOKEN_FALSE = auto()
    TOKEN_NONE = auto()
    TOKEN_NIL = auto()
    TOKEN_OR = auto()

    # PUNCTUATION + OPERATORS
    TOKEN_LEFT_PAREN = auto()
    TOKEN_RIGHT_PAREN = auto()
    TOKEN_COLON = auto()
    TOKEN_COMMA = auto()
    TOKEN_EQUAL = auto()
    TOKEN_ARROW = auto()
    TOKEN_MINUS = auto()


class Token:
    # Position = current char in file
    def __init__(
        self, TokType: TokenType, Line: int, Column: int, Position: int, Lexeme: str
    ):
        self.TokType = TokType
        self.Line = Line
        self.Column = Column
        self.Position = Position
        self.Lexeme = Lexeme


def NewToken(
    TokType: TokenType, Line: int, Column: int, Position: int, Lexeme: str
) -> Token:
    Tok = Token(TokType, Line, Column, Position, Lexeme)
    return Tok
