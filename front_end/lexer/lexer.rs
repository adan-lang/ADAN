use super::tokens::{Token, TokenType};

struct Lexer {
    pub source: String,
    pub position: usize,
}

impl Lexer {
    // offset as usize to adapt to cpu bit limit for optionally
    // peeking n characters ahead.
    fn peek(&self, offset: usize) -> Option<u8> {
        self.source.as_bytes().get(self.position + offset).copied()
    }

    // just wrapper to peek 1 character ahead.
    fn peek_next(&self) -> Option<u8> {
        self.peek(1)
    }

    // advances the cursor up 1 position.
    fn consume(&self) {}

    // public interface for tokenizing a string.
    pub fn lex(&self) {}
}
