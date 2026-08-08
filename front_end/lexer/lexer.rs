use super::tokens::{Token, TokenType};

pub struct Lexer {
    pub source: Vec<char>,
    pub position: usize,
}

impl Lexer {
    pub fn new(source: String) -> Self {
        Lexer {
            source: source.chars().collect(),
            position: 0,
        }
    }

    // core

    fn peek(&self, offset: usize) -> char {
        *self.source.get(self.position + offset).unwrap_or(&'\0')
    }

    fn peek_next(&self) -> char {
        self.peek(1)
    }

    fn current(&self) -> char {
        self.peek(0)
    }

    fn consume(&mut self) -> char {
        let c = self.current();
        self.position += 1;
        c
    }

    fn at_end(&self) -> bool {
        self.position >= self.source.len()
    }

    // pub

    pub fn lex(&mut self) -> Vec<Token> {
        let tokens = Vec::new();

        while !self.at_end() {
            // @todo actually do shit lol

            self.consume();
        }

        tokens
    }

    // helpers

    fn read_number(&mut self) -> Token {}

    fn read_ident(&mut self) -> Token {}
}
