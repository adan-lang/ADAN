#include <iostream>

typedef enum TokenType
{
    /* Special */
    TOKEN_ERROR,
    TOKEN_EOF,
    TOKEN_IDENTIFIER,

    /* Keywords */
    TOKEN_VAR,
    TOKEN_GLOBAL,
    TOKEN_READONLY,
    TOKEN_IF,
    TOKEN_FUNC,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_REPEAT,
    TOKEN_UNTIL,
    TOKEN_BREAK,
    TOKEN_PASS,
    TOKEN_IN,

    /* Symbols */
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_EQUALS,
    TOKEN_NOT,
    TOKEN_SUB,
    TOKEN_ADD,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOPLON,
} TokenType;

typedef struct Token
{
    std::string pos;
    int line;
    int col;
    std::string lexeme;
    TokenType* type;
} Token;