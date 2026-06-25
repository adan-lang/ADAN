#pragma

#include <unordered_map>
#include      <iostream>
#include        <string>

typedef enum TokenType
{
    /* Special */
    TOKEN_ERROR,
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_WHITESPACE,

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
    TOKEN_INCLUDE,
    TOKEN_USING,
    TOKEN_EXPORT,

    /* Types */
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_NUMBER,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_BOOL,
    TOKEN_NONE,
    TOKEN_VOID,
    TOKEN_AUTO,

    /* Symbols */
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_QUOTE,
    TOKEN_APOSTRAPHE,
    TOKEN_COMMENT,
    TOKEN_MULTILINE_COMMENT,

    /* Binary Operators */
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_POW,
    TOKEN_DIV,
    TOKEN_REMAINDER,
    TOKEN_EQUALS,
    TOKEN_NOT_EQUALS,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_ASSIGN,
    TOKEN_ADD_ASSIGN,
    TOKEN_SUB_ASSIGN,
    TOKEN_MUL_ASSIGN,
    TOKEN_POW_ASSIGN,
    TOKEN_DIV_ASSIGN,
    TOKEN_REMAINDER_ASSIGN,
    TOKEN_FLOOR_DIV,
    TOKEN_FLOOR_DIV_ASSIGN,
    TOKEN_ELVIS, // ?: -- stolen from kotlin
    
    /* Unary Operators */
    TOKEN_NOT_NULL_ASSERT, // !!
    TOKEN_NOT,
    TOKEN_NEGATE
} TokenType;

typedef struct Token
{
    int pos;
    int line;
    int col;
    std::string lexeme;
    TokenType type;
} Token;

extern std::unordered_map<std::string, TokenType> Keywords;

extern std::unordered_map<std::string, TokenType> Types;

extern std::unordered_map<std::string, TokenType> Symbols;