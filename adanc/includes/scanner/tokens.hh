#pragma once

#include <cstdint>
#include <string>

#include "scanner-helpers.hh"

enum class TokenKind : std::uint8_t
{
    // Special
    Ident,
    Eof,

    // Literals
    Int,
    Float,
    String,
    Bool,
    TemplateString,   // Raw string segment
    InterpolateStart, // ${
    InterpolateEnd,   // }

    // Symbols
    Plus,        // +
    Minus,       // -
    Multiply,    // *
    Divide,      // /
    Power,       // ^
    Modulo,      // %
    GreaterThan, // >
    LessThan,    // <
    Equal,       // =
    SemiColon,   // ;
    Hashtag,     // #
    Dollar,      // $
    LParen,      // (
    RParen,      // )
    LBracket,    // [
    RBracket,    // ]
    LBrace,      // {
    RBrace,      // }
    Quote,       // "
    Apostrophe,  // '
    Ellipsis,    // ...
    Tick,        // `
    Comma,       // ,
    Not,         // !

    // Variant Symbols
    Arrow,          // ->
    BackArrow,      // <-
    Increment,      // ++
    Decrement,      // --
    PlusAssign,     // +=
    MinusAssign,    // -=
    MultiplyAssign, // *=
    DivideAssign,   // /=
    ModuloAssign,   // %=
    EqualEqual,     // ==
    NotEqualEqual,  // !==
    GreaterEqual,   // >=
    LessEqual,      // <=

    // Keywords
    Local,
    Function,
    If,
    While,
    For,
    In,

    // Built-ins (Keywords)
    Print,
};

struct Token
{
    TokenKind kind;
    std::string lexeme;
    
    int line{};
    int column{};

    [[nodiscard]] std::string to_string() const
    {
        return "Token { kind: \033[1;32m" + std::string(token_kind_name(kind)) + "\033[0m, lexeme: \033[1;33m\"" + lexeme + "\"\033[0m, line: " + std::to_string(line) + ", column: " + std::to_string(column) + " }";
    }
};