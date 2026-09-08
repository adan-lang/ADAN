#pragma once

#include <format>
#include <string>

enum class TokenKind
{
    IDENT,    /*  A-Z|a-z|0-9|_  */
    STRING,   /* "A-Z|a-z|0-9|_" */
    NUMBER,   /*      0 - 9      */
    ADD,      /*        +        */
    SUBTRACT, /*        -        */
    MULTIPLY, /*        *        */
    DIVIDE,   /*        /        */
    MODULO,   /*        %        */
    EQUALS,   /*        =        */
    AT,       /*        @        */
    PERIOD,   /*        .        */
    QUOTE,    /*        "        */
    LPAREN,   /*        (        */
    RPAREN,   /*        )        */
};

static inline std::string kind_to_string(TokenKind kind)
{
    switch (kind)
    {
    case TokenKind::STRING:
        return "STRING";
    case TokenKind::NUMBER:
        return "NUMBER";
    case TokenKind::ADD:
        return "ADD";
    case TokenKind::SUBTRACT:
        return "SUBTRACT";
    case TokenKind::MULTIPLY:
        return "MULTIPLY";
    case TokenKind::DIVIDE:
        return "DIVIDE";
    case TokenKind::MODULO:
        return "MODULO";
    case TokenKind::EQUALS:
        return "EQUALS";
    case TokenKind::AT:
        return "AT";
    case TokenKind::PERIOD:
        return "PERIOD";
    case TokenKind::QUOTE:
        return "QUOTE";
    case TokenKind::LPAREN:
        return "LPAREN";
    case TokenKind::RPAREN:
        return "RPAREN";
    case TokenKind::IDENT:
        return "IDENT";
    default:
        return "UNKNOWN";
    }
}

struct Token
{
    TokenKind kind;
    std::string value;
    int line;
    int column;

    std::string to_string() const
    {
        return std::format("Token({}, \"{}\", {}:{})", kind_to_string(kind), value, line, column);
    }
};