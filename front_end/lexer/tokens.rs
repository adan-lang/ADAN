#[derive(Debug)]
pub enum Token {
    // Special
    Eof,

    // Binary Ops
    Mul,
    Div,
    Add,
    Sub,

    // Symbols
    LParen,
    RParen,

    // Literals
    Int(i32),
}
