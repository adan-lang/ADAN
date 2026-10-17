#pragma once

#include <cstdint>
#include <string>
#include <string_view>

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

constexpr std::string_view token_kind_name(const TokenKind kind)
{
    switch (kind)
    {
    case TokenKind::Ident:
        return "Ident";
    case TokenKind::Eof:
        return "Eof";
    case TokenKind::Int:
        return "Int";
    case TokenKind::Float:
        return "Float";
    case TokenKind::String:
        return "String";
    case TokenKind::Bool:
        return "Bool";
    case TokenKind::TemplateString:
        return "TemplateString";
    case TokenKind::InterpolateStart:
        return "InterpolateStart";
    case TokenKind::InterpolateEnd:
        return "InterpolateEnd";
    case TokenKind::Plus:
        return "Plus";
    case TokenKind::Minus:
        return "Minus";
    case TokenKind::Multiply:
        return "Multiply";
    case TokenKind::Divide:
        return "Divide";
    case TokenKind::Modulo:
        return "Modulo";
    case TokenKind::GreaterThan:
        return "GreaterThan";
    case TokenKind::LessThan:
        return "LessThan";
    case TokenKind::Equal:
        return "Equal";
    case TokenKind::SemiColon:
        return "SemiColon";
    case TokenKind::Hashtag:
        return "Hashtag";
    case TokenKind::Dollar:
        return "Dollar";
    case TokenKind::LParen:
        return "LParen";
    case TokenKind::RParen:
        return "RParen";
    case TokenKind::LBracket:
        return "LBracket";
    case TokenKind::RBracket:
        return "RBracket";
    case TokenKind::LBrace:
        return "LBrace";
    case TokenKind::RBrace:
        return "RBrace";
    case TokenKind::Quote:
        return "Quote";
    case TokenKind::Apostrophe:
        return "Apostrophe";
    case TokenKind::Ellipsis:
        return "Ellipsis";
    case TokenKind::Tick:
        return "Tick";
    case TokenKind::Comma:
        return "Comma";
    case TokenKind::Arrow:
        return "Arrow";
    case TokenKind::BackArrow:
        return "BackArrow";
    case TokenKind::Increment:
        return "Increment";
    case TokenKind::Decrement:
        return "Decrement";
    case TokenKind::PlusAssign:
        return "PlusAssign";
    case TokenKind::MinusAssign:
        return "MinusAssign";
    case TokenKind::MultiplyAssign:
        return "MultiplyAssign";
    case TokenKind::DivideAssign:
        return "DivideAssign";
    case TokenKind::ModuloAssign:
        return "ModuloAssign";
    case TokenKind::EqualEqual:
        return "EqualEqual";
    case TokenKind::GreaterEqual:
        return "GreaterEqual";
    case TokenKind::LessEqual:
        return "LessEqual";
    case TokenKind::Local:
        return "Local";
    case TokenKind::Function:
        return "Function";
    case TokenKind::If:
        return "If";
    case TokenKind::While:
        return "While";
    case TokenKind::For:
        return "For";
    case TokenKind::In:
        return "In";
    case TokenKind::Print:
        return "Print";
    }

    return "Unknown";
}

struct Token
{
    TokenKind kind;
    std::string lexeme;

    [[nodiscard]] std::string to_string() const
    {
        return "Token { kind: \033[1;32m" + std::string(token_kind_name(kind)) + "\033[0m, lexeme: \033[1;33m\"" + lexeme + "\"\033[0m }";
    }
};