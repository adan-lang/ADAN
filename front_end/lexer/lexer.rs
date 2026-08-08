use super::tokens::{Token, TokenType};

pub struct Lexer {
    pub source: String,
    pub position: usize,
}

impl Lexer {
    fn peek(&self, offset: usize) -> char {
        self.source
            .chars()
            .nth(self.position + offset)
            .unwrap_or('\0')
    }

    fn peek_next(&self) -> char {
        self.peek(1)
    }

    fn consume(&mut self) {
        self.position += 1;
    }

    pub fn lex(&self) -> Vec<Token> {
        let tokens: Vec<Token> = vec![];

        tokens
    }
}
