#pragma once

#include <cctype>
#include <cstddef>
#include <format>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "diagnostics.hh"
#include "tokens.hh"

const std::unordered_map<TokenKind, std::string_view> keywords = {
    {TokenKind::Local, "local"},
    {TokenKind::Function, "function"},
    {TokenKind::If, "if"},
    {TokenKind::While, "while"},
    {TokenKind::For, "for"},
    {TokenKind::In, "in"},
};

class Lexer {
    std::string_view source{};

    Diagnostics &diagnostics;

    std::size_t position{};

    int line{1};
    int column{1};
    int token_line{1};
    int token_column{1};

    std::queue<Token> pending{};

  public:
    explicit Lexer(std::string_view source_text, Diagnostics &diagnostics, int start_line = 1, int start_column = 1) : source{source_text}, diagnostics{diagnostics}, line{start_line}, column{start_column} {}

    Token next()
    {
        if (!pending.empty())
        {
            auto token = pending.front();
            pending.pop();

            return token;
        }

        Token token = lex();
        if (token.line == 0 && token.column == 0)
        {
            token.line = token_line;
            token.column = token_column;
        }
        return token;
    }

  private:
    Token lex();

    [[noreturn]] void report_error(std::string message)
    {
        diagnostics.error(std::move(message), token_line, token_column);
    }

    void advance()
    {
        if (!is_eof())
        {
            if (source[position++] == '\n')
            {
                ++line;
                column = 1;
            }
            else
                ++column;
        }
    }

    [[nodiscard]] char peek(const std::size_t offset = 0) const
    {
        return (position + offset >= source.length()) ? '\0' : source[position + offset];
    }

    void skip_spaces()
    {
        while (!is_eof() && std::isspace(peek()))
            advance();
    }

    void skip_comments()
    {
        while (!is_eof())
        {
            skip_spaces();

            if (peek() == ';' && peek(1) == '-' && peek(2) == '-')
            {
                const int comment_line{line};
                const int comment_column{column};
                advance();
                advance();
                advance();

                bool terminated{};
                while (!is_eof())
                {
                    if (peek() == '-' && peek(1) == '-' && peek(2) == ';')
                    {
                        advance();
                        advance();
                        advance();
                        terminated = true;
                        break;
                    }

                    advance();
                }

                if (!terminated)
                    diagnostics.error("Unterminated block comment", comment_line, comment_column);
            }
            else if (peek() == ';')
            {
                while (!is_eof() && peek() != '\n')
                    advance();
            }
            else
            {
                break;
            }
        }
    }

    char unescape(const char c)
    {
        switch (c)
        {
        case 'n':
            return '\n'; // newline
        case 't':
            return '\t'; // horizontal tab
        case 'r':
            return '\r'; // carriage return
        case '\\':
            return '\\'; // backslash
        case '"':
            return '"'; // double quote
        case '\'':
            return '\''; // single quote
        case '`':
            return '`'; // backtick
        case '0':
            return '\0'; // null
        case 'b':
            return '\b'; // backspace
        case 'v':
            return '\v'; // vertical tab
        default:
            if (c == '\0')
                return c;

            diagnostics.warning(std::format("Unknown escape sequence '\\{}'", c), line, column);
            return c;
        }
    }

    [[nodiscard]] bool is_eof() const
    {
        return position >= source.length();
    }
};