#pragma once

#include <cstdint>
#include <string>

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

    // Symbols
    Plus,        // +
    Minus,       // -
    Multiply,    // *
    Divide,      // /
    Modulo,      // %
    GreaterThan, // >
    LessThan,    //
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
    GreaterEqual,   // >=
    LessEqual,      // <=
    Interpolate,    // ${

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

    std::string to_string() const
    {
        std::string kind_str;

        switch (kind)
        {
        case TokenKind::Ident:
            kind_str = "Ident";
            break;
        case TokenKind::Eof:
            kind_str = "Eof";
            break;
        case TokenKind::Int:
            kind_str = "Int";
            break;
        case TokenKind::Float:
            kind_str = "Float";
            break;
        case TokenKind::String:
            kind_str = "String";
            break;
        case TokenKind::Bool:
            kind_str = "Bool";
            break;
        case TokenKind::Plus:
            kind_str = "Plus";
            break;
        case TokenKind::Minus:
            kind_str = "Minus";
            break;
        case TokenKind::Multiply:
            kind_str = "Multiply";
            break;
        case TokenKind::Divide:
            kind_str = "Divide";
            break;
        case TokenKind::Modulo:
            kind_str = "Modulo";
            break;
        case TokenKind::GreaterThan:
            kind_str = "GreaterThan";
            break;
        case TokenKind::LessThan:
            kind_str = "LessThan";
            break;
        case TokenKind::Equal:
            kind_str = "Equal";
            break;
        case TokenKind::SemiColon:
            kind_str = "SemiColon";
            break;
        case TokenKind::Hashtag:
            kind_str = "Hashtag";
            break;
        case TokenKind::Dollar:
            kind_str = "Dollar";
            break;
        case TokenKind::LParen:
            kind_str = "LParen";
            break;
        case TokenKind::RParen:
            kind_str = "RParen";
            break;
        case TokenKind::LBracket:
            kind_str = "LBracket";
            break;
        case TokenKind::RBracket:
            kind_str = "RBracket";
            break;
        case TokenKind::LBrace:
            kind_str = "LBrace";
            break;
        case TokenKind::RBrace:
            kind_str = "RBrace";
            break;
        case TokenKind::Quote:
            kind_str = "Quote";
            break;
        case TokenKind::Apostrophe:
            kind_str = "Apostrophe";
            break;
        case TokenKind::Ellipsis:
            kind_str = "Ellipsis";
            break;
        case TokenKind::Tick:
            kind_str = "Tick";
            break;
        case TokenKind::Comma:
            kind_str = "Comma";
            break;
        case TokenKind::Arrow:
            kind_str = "Arrow";
            break;
        case TokenKind::BackArrow:
            kind_str = "BackArrow";
            break;
        case TokenKind::Increment:
            kind_str = "Increment";
            break;
        case TokenKind::Decrement:
            kind_str = "Decrement";
            break;
        case TokenKind::PlusAssign:
            kind_str = "PlusAssign";
            break;
        case TokenKind::MinusAssign:
            kind_str = "MinusAssign";
            break;
        case TokenKind::MultiplyAssign:
            kind_str = "MultiplyAssign";
            break;
        case TokenKind::DivideAssign:
            kind_str = "DivideAssign";
            break;
        case TokenKind::ModuloAssign:
            kind_str = "ModuloAssign";
            break;
        case TokenKind::EqualEqual:
            kind_str = "EqualEqual";
            break;
        case TokenKind::GreaterEqual:
            kind_str = "GreaterEqual";
            break;
        case TokenKind::LessEqual:
            kind_str = "LessEqual";
            break;
        case TokenKind::Interpolate:
            kind_str = "Interpolate";
            break;
        case TokenKind::Local:
            kind_str = "Local";
            break;
        case TokenKind::Function:
            kind_str = "Function";
            break;
        case TokenKind::If:
            kind_str = "If";
            break;
        case TokenKind::While:
            kind_str = "While";
            break;
        case TokenKind::For:
            kind_str = "For";
            break;
        case TokenKind::In:
            kind_str = "In";
            break;
        case TokenKind::Print:
            kind_str = "Print";
            break;
        }

        return "Token { kind: \033[1;32m" + kind_str + "\033[0m, lexeme: \033[1;33m\"" + lexeme + "\"\033[0m }";
    }
};