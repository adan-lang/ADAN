#pragma once

#include <unordered_map>
#include <iostream>
#include <variant>
#include <format>
#include <string>
#include <vector>
#include <cctype>
#include <optional>

#include "errors.hh"
#include "tokens.hh"
#include "lexer-errors.hh"

typedef struct LexerType
{
    std::string source;
    int pos;
    int line;
    int col;
    bool abort;
} LexerType;

class Lexer
{
private:
    std::string source; // the source code used for analysis
    int pos;            // the current position in the file (character)
    int line;           // the line you are on in the file (y-axis)
    int col;            // the column you are in the file (x-axis)
    bool abort;         // the state of panic; true = CRITICAL ERROR

    /**
     * 
     * reading the *current* character of `self->source`, optionally
     *  peeking `offset` character(s) ahead.
     */
    char peek(
        int offset = 0
    );

    /**
     * 
     * read 1 character ahead of `current`. using `peek(1)`
     *  reaches the same results.
     */
    char peek_next();
    
    /**
     * 
     * shifts `self->pos` (current position in file) +1 and changes
     *  `self->line`/`self->col` accordingly.
     * 
     * returns the character it advances past.
     */
    char consume();

    /**
     * 
     * returns a boolean state based off whether the provided char is
     *  equivalent to the next char.
     * 
     * always runs `self->consume()` regardless of state.
     */
    bool match(
        char to_match
    );

    /**
     * 
     * create a new lexer error. although doesn't hault the compiler;
     *  it will log 1 by 1 on compile time.
     */
    void emit_error(
        ErrorCode code,
        int line,
        int col_start,
        int col_end,
        std::optional<std::string> hint_override = std::nullopt
    );

    /**
     * 
     * skip token scanning for anything that is inside of a comment,
     *  shifts current position in file for you.
     */
    void scan_comments(
        std::vector<Token>& tokens
    );

    /**
     * 
     * skip any whitespace characters for you, similarly to
     *  `self->scan_comments()`.
     */
    void scan_whitespace(
        std::vector<Token>& tokens
    );

public:
    Lexer(
        LexerType lexer
    );
    
    ~Lexer();

    /**
     * 
     * the lexical scanner: takes in a file and outputs a stream of
     *  tokens to later be parsed.
     */
    std::vector<Token> scan();
};