#include <unordered_map>

#include "tokens.hh"

std::unordered_map<std::string, TokenType> Keywords = {
    /* Keywords */
    {"var",      TOKEN_VAR},        // Defining a new variable
    {"global",   TOKEN_GLOBAL},     // Allowing a variable to be used in any scope above or below its initial scope
    {"readonly", TOKEN_READONLY},   // Creating an immutable variable
    {"if",       TOKEN_IF},
    {"func",     TOKEN_FUNC},
    {"for",      TOKEN_FOR},
    {"while",    TOKEN_WHILE},      // Repeat while x condition is true.
    {"repeat",   TOKEN_REPEAT},     // Repeat until x condition is met.
    {"until",    TOKEN_UNTIL},      // ...
    {"break",    TOKEN_BREAK},
    {"pass",     TOKEN_PASS},       // Skip the loop's current iteration.
    {"in",       TOKEN_IN},         // Used for key-value loops. `for <Key>?, <Value>? in <Array> { ... }`
    {"include",  TOKEN_INCLUDE},    // Include source files using a file-path system. Example: "src/utils/parser.adn"
    {"using",    TOKEN_USING},      // Modular importing system, similar to C#. Resolve libraries via name rather than path.
    {"export",   TOKEN_EXPORT},     // Export functions, variables, etc. to be used in other files. (Required when creating libraries)
    {"todo",     TOKEN_TODO},       // Works similarly to Python's `pass` keyword, and Gleam's `todo` keyword.
};

std::unordered_map<std::string, TokenType> Types = {
    /* Types */
    {"String", TOKEN_STRING},
    {"Char",   TOKEN_CHAR},
    {"Number", TOKEN_NUMBER},
    {"Int",    TOKEN_INTEGER},
    {"Float",  TOKEN_FLOAT},
    {"Bool",   TOKEN_BOOL},
    {"None",   TOKEN_NONE},
    {"Void",   TOKEN_VOID},
    {"Auto",   TOKEN_AUTO},
};

std::unordered_map<std::string, TokenType> Symbols = {
    /* Symbols */
    {",",  TOKEN_COMMA},
    {":",  TOKEN_COLON},
    {"(",  TOKEN_LPAREN},
    {")",  TOKEN_RPAREN},
    {"{",  TOKEN_LBRACE},
    {"}",  TOKEN_RBRACE},
    {"[",  TOKEN_LBRACKET},
    {"]",  TOKEN_RBRACKET},
    {";",  TOKEN_SEMICOLON},
    {"\"", TOKEN_QUOTE},
    {"'",  TOKEN_APOSTRAPHE},

    /* Binary Operators */
    {"+",   TOKEN_ADD},
    {"-",   TOKEN_SUB},
    {"*",   TOKEN_MUL},
    {"**",  TOKEN_POW},
    {"/",   TOKEN_DIV},
    {"%",   TOKEN_REMAINDER},
    {"==",  TOKEN_EQUALS},
    {"!=",  TOKEN_NOT_EQUALS},
    {"<",   TOKEN_LESS},
    {"<=",  TOKEN_LESS_EQUAL},
    {">",   TOKEN_GREATER},
    {">=",  TOKEN_GREATER_EQUAL},
    {"and", TOKEN_AND},
    {"or",  TOKEN_OR},
    {"=",   TOKEN_ASSIGN},
    {"+=",  TOKEN_ADD_ASSIGN},
    {"-=",  TOKEN_SUB_ASSIGN},
    {"*=",  TOKEN_MUL_ASSIGN},
    {"**=", TOKEN_POW_ASSIGN},
    {"/=",  TOKEN_DIV_ASSIGN},
    {"%=",  TOKEN_REMAINDER_ASSIGN},
    {"//",  TOKEN_FLOOR_DIV},
    {"//=", TOKEN_FLOOR_DIV_ASSIGN},
    {"?:",  TOKEN_ELVIS},              // Executes y if x fails. Example: var my_variable = <Expression> ?: <Expression2> # <Expression2> never runs if <Expression> succeeds.
    
    /* Unary Operators */
    {"!!", TOKEN_NOT_NULL_ASSERT},   // Tells the compiler this value (unknown to be null at runtime) that it will never be null. Example: <Condition>!!
    {"!",  TOKEN_NOT},               // Flips the truthiness of a Bool. Example: !true -> false and !!true -> true
    {"-",  TOKEN_NEGATE},            // Changes the sign to the inverse of the current sign. Example: --2 -> flip -2 -> 2
};