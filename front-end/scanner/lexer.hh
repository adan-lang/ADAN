#include <iostream>
#include   <format>
#include   <string>
#include   <vector>
#include  <variant>

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
                    throw std::runtime_error(
                        std::format("[error]: never-ending multi-line comment found on line {}", line)
                    );
                    abort = true;
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

            if (current == '\0')
            {
                tokens.push_back(Token{
                    pos,
                    line, col,
                    std::string{},
                    TOKEN_EOF
                });

                break;
            }

            /**
             * 
             * @todo: lex the following: IDENTIFIERS, KEYWORDS, SYMBOLS,
             *                           NUMBERS (Integers and Floating Points)
             */

             consume();
        }

        return tokens;
    }
};