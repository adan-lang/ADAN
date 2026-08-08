#[derive(Debug)]
pub enum TokenType {
    TokenEof,

    //  KEYWORDS
    TokenLet,

    //  TYPES
    TokenInt,
    TokenFloat,

    //  SYMBOLS
    TokenEqual,
    TokenColon,
    TokenSub,
    TokenAdd, // also is used for unary negation
    TokenMul,
    TokenDiv,
    TokenMod,
    TokenLeftParen,
    TokenRightParen,
}

#[derive(Debug)]
pub struct Token {
    lexeme: String,
    token_type: TokenType,
    line: u32,
    column: u32,
}
