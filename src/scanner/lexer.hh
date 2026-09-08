#pragma once

#include <string>
#include <vector>
#include <cctype>

#include "token.hh"

class Lexer
{
public:
    std::vector<Token> lex()
    {
        std::vector<Token> tokens;

        while (!eof())
        {
            skip_ws();
            char curr = current();

            switch (curr)
            {
            case '+':
                tokens.emplace_back(Token{
                    TokenKind::ADD,
                    "+",
                    line,
                    column,
                });
                consume();
                break;

            case '-':
                tokens.emplace_back(Token{
                    TokenKind::SUBTRACT,
                    "-",
                    line,
                    column,
                });
                consume();
                break;

            case '*':
                tokens.emplace_back(Token{
                    TokenKind::MULTIPLY,
                    "*",
                    line,
                    column,
                });
                consume();
                break;

            case '/':
                tokens.emplace_back(Token{
                    TokenKind::DIVIDE,
                    "/",
                    line,
                    column,
                });
                consume();
                break;

            case '%':
                tokens.emplace_back(Token{
                    TokenKind::MODULO,
                    "%",
                    line,
                    column,
                });
                consume();
                break;

            case '=':
                tokens.emplace_back(Token{
                    TokenKind::EQUALS,
                    "=",
                    line,
                    column,
                });
                consume();
                break;

            case '@':
                tokens.emplace_back(Token{
                    TokenKind::AT,
                    "@",
                    line,
                    column,
                });
                consume();
                break;

            case '.':
                tokens.emplace_back(Token{
                    TokenKind::PERIOD,
                    ".",
                    line,
                    column,
                });
                consume();
                break;

            case '(':
                tokens.emplace_back(Token{
                    TokenKind::LPAREN,
                    "(",
                    line,
                    column,
                });
                consume();
                break;

            case ')':
                tokens.emplace_back(Token{
                    TokenKind::RPAREN,
                    ")",
                    line,
                    column,
                });
                consume();
                break;

            case '"':
                tokens.emplace_back(lex_string());
                break;

            default:
                if (isdigit(curr))
                    tokens.emplace_back(lex_number());
                else if (isalpha(curr) || curr == '_')
                    tokens.emplace_back(lex_identifier());
                else
                    consume(); // skip unknown char for now
                break;
            }
        }

        return tokens;
    }

    Lexer(std::string source_file);

    ~Lexer();

private:
    std::string source_code;
    int pos{};
    int line{};
    int column{};

    /**
     *
     * @brief Returns the current character.
     */
    char current()
    {
        return source_code[pos];
    }

    /**
     *
     * @brief Peak *n* characters ahead, or 1 if unsupplied.
     */
    char peek(int offset = 1)
    {
        return source_code[pos + offset];
    }

    /**
     *
     * @brief Return the current position, then advance by 1.
     */
    char consume()
    {
        if (!eof())
        {
            char curr = source_code[pos++];

            if (curr == '\n')
            {
                line++;
                column = 0;
            }
            else
            {
                column++;
            }

            return curr;
        }

        return '\0';
    }

    /**
     *
     * @brief Returns a boolean state depending on if it has reached
     *        the end of the input stream.
     */
    bool eof()
    {
        return pos >= (int)source_code.length();
    }

    /**
     *
     * @brief Skip all whitespace-identified characters until the next
     *        non-whitespace character.
     */
    void skip_ws()
    {
        while (!eof() && isspace(current()))
        {
            consume();
        }
    }

    // private tokenizing stuff

    Token lex_number()
    {
        int start_line = line;
        int start_column = column;
        std::string value;

        while (!eof() && (isdigit(current()) || current() == '.'))
        {
            value += consume();
        }

        return Token{
            TokenKind::NUMBER,
            value,
            start_line,
            start_column,
        };
    }

    Token lex_string()
    {
        int start_line = line;
        int start_column = column;
        std::string value;

        consume();

        while (!eof() && current() != '"')
        {
            if (current() == '\\' && peek() == '"') // escaped quote \"
            {
                consume();
                value += consume();
            }
            else
            {
                value += consume();
            }
        }

        if (eof())
        {
            // unterminated string, do error stuff later lol
        }
        else
        {
            consume();
        }

        return Token{
            TokenKind::STRING,
            value,
            start_line,
            start_column,
        };
    }

    Token lex_identifier()
    {
        int start_line = line;
        int start_column = column;
        std::string value;

        while (!eof() && (isalnum(current()) || current() == '_'))
        {
            value += consume();
        }

        return Token{
            TokenKind::IDENT,
            value,
            start_line,
            start_column,
        };
    }
};