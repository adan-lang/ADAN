use super::tokens::{Token, TokenType};

pub struct Lexer {
    pub source: String,
    pub position: usize,
}

impl Lexer {
    fn peek(&self, offset: usize) -> Option<u8> {
        self.source.as_bytes().get(self.position + offset).copied()
    }

    fn peek_next(&self) -> Option<u8> {
        self.peek(1)
    }

    fn consume(&self) {}

    pub fn lex(&self) -> Vec<Token> {
        let tokens: Vec<Token> = vec![];

        tokens
    }
}
