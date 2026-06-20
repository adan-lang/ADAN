#pragma

#include      <iostream>
#include        <format>
#include        <string>
#include        <vector>
#include       <variant>
#include        <cctype>
#include <unordered_map>

#include            "../tokens.hh"
#include "../../utility/errors.hh"

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
     * @category requirement
     * 
     * reading the *current* character of `self->source`, optionally
     *  peeking `offset` character(s) ahead.
     * 
     * @param offset
     */
    char peek(
        int offset = 0
    )
    {
        if ((pos + offset) >= source.length())
        {
            return '\0';
        }

        else
        {
            return source[pos + offset];
        }
    }

    /**
     * 
     * @category convenience
     * @details convienence wrapper; easier to read.
     * 
     * read 1 character ahead of `current`. using `peek(1)`
     *  reaches the same results.
     */
    char peek_next()
    {
        return peek(1);
    }
    
    /**
     * 
     * @category requirement
     * 
     * shifts `self->pos` (current position in file) +1 and changes
     *  `self->line`/`self->col` accordingly.
     * 
     * returns the character it advances past.
     */
    char consume()
    {
        char current_char = peek();
        
        if (current_char == '\n')
        {
            line += 1;
            col = 1;
        }

        else
        {
            col += 1;
        }

        pos += 1;
    }

    /**
     * 
     * @category requirement
     * 
     * returns a boolean state based off whether the provided char is
     *  equivalent to the next char.
     * 
     * always runs `self->consume()` regardless of state.
     * 
     * @param to_match
     */
    bool match(
        char to_match
    ) {
        consume();
        return peek() != to_match;
    }
    
    /**
     * 
     * @category helper
     * 
     * skip token scanning for anything that is inside of a comment,
     *  shifts current position in file for you.
     */
    void skip_comments()
    {
        if (peek() == '#')
        {
            consume();

            while (peek() != '\n' && peek() != '\0')
            {
                consume();
            }
        }

        else if (peek() == '#' && peek_next() == '#')
        {
            consume();
            consume();

            while (!(peek() == '#' && peek_next() == '#'))
            {
                if (peek() == '\0')
                {
                    abort = true;
                    
                    Error non_terminating_comment;

                    non_terminating_comment.message = "Never ending multiline comment found";
                    non_terminating_comment.severity = ErrorSeverity::SEVERITY_FATAL;
                    non_terminating_comment.line = line;
                    non_terminating_comment.line = col;

                    report(non_terminating_comment);
                }

                consume();
            }

            consume();
            consume();
        }
    }

    /**
     * 
     * @category helper
     * 
     * skip any whitespace characters for you, similarly to
     *  `self->skip_comments()`.
     */
    void skip_whitespace()
    {
        while (
            peek() == ' '  || peek() == '\t' ||
            peek() == '\r' || peek() == '\n'
        ) {
            consume();
        }
    }

public:
    Lexer(
        LexerType lexer
    );
    
    ~Lexer();

    /**
     * 
     * the lexical scanner: takes in a file and outputs a stream of
     *  tokens to later be parsed.
     * 
     * @param LexerType* lexer
     */
    std::vector<Token> scan()
    {
        std::vector<Token> tokens;

        for (;;)
        {
            for (;;)
            {
                int current_pos{pos};

                skip_whitespace();
                skip_comments();
                
                if (pos == current_pos)
                {
                    break;
                }
            }

            char current = peek();

            int start_line = line;
            int start_col = col;

            if (current == '\0')
            {
                tokens.push_back(Token{
                    pos,
                    start_line,
                    start_col,
                    std::string{},
                    TOKEN_EOF
                });

                break;
            }

            if (std::isalpha(current) || current == '_')
            {
                int start = pos;
                
                consume();

                while (std::isalnum(peek()) || peek() == '_')
                {
                    consume();
                }

                std::string lexeme = source.substr(start, pos - start);

                auto iterator = Keywords.find(lexeme);
                auto type_iterator = Types.find(lexeme);
                
                TokenType token_type;

                if (iterator != Keywords.end())
                    token_type = static_cast<TokenType>(iterator->second);
                else if (type_iterator != Types.end())
                    token_type = static_cast<TokenType>(type_iterator->second);
                else
                    token_type = TOKEN_IDENTIFIER;

                tokens.push_back(Token{
                    pos,
                    start_line,
                    start_col,
                    lexeme,
                    token_type
                });

                continue;
            }

            if (std::isdigit(current))
            {
                int start = pos;
                bool is_float = false;
                
                consume();

                while (std::isdigit(peek()))
                {
                    consume();
                }

                if (peek() == '.')
                {
                    if (std::isdigit(peek_next()))
                    {
                        is_float = true;
                        
                        consume();
                        
                        while (std::isdigit(peek()))
                        {
                            consume();
                        }
                    }

                    else
                    {
                        abort = true;

                        Error incomplete_floating_point;

                        incomplete_floating_point.message = "Incomplete floating point value found; meaning you have `<INTEGER>.`, without anything afterwards found";
                        incomplete_floating_point.severity = ErrorSeverity::SEVERITY_ERROR;
                        incomplete_floating_point.line = line;
                        incomplete_floating_point.line = col;

                        report(incomplete_floating_point);
                    }
                }
                
                std::string lexeme = source.substr(start, pos - start);

                tokens.push_back(Token{
                    pos,
                    start_line,
                    start_col,
                    lexeme,
                    !is_float ? TOKEN_INTEGER : TOKEN_FLOAT
                });

                continue;
            }

            /**
             * 
             * @todo: lex the following: SYMBOLS
             */

            consume();
        }

        return tokens;
    }
};