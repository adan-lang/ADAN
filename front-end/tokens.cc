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
    {"and",      TOKEN_AND},
    {"or",       TOKEN_OR},
    {"else",     TOKEN_ELSE},
    {"lambda",   TOKEN_LAMBDA},
    {"true",     TOKEN_TRUE},
    {"false",    TOKEN_FALSE}
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