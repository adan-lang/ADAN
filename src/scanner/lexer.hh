#pragma once

#include "token.hh"

#include <cctype>
#include <queue>
#include <unordered_map>

const std::unordered_map<TokenKind, std::string_view> keywords = {
    {TokenKind::Local, "local"},
    {TokenKind::Function, "function"},
    {TokenKind::If, "if"},
    {TokenKind::While, "while"},
    {TokenKind::For, "for"},
    {TokenKind::In, "in"},
};

class Lexer
{
public:
    explicit Lexer(std::string_view source) : source{source} {}

    Token next()
    {
        if (!pending_tokens.empty())
        {
            // std::println("draining pending: size {}", pending_tokens.size());
            auto token = pending_tokens.front();
            pending_tokens.pop();
            return token;
        }

        return lex();
    }

private:
    std::string_view source{};
    std::size_t position{};
    std::queue<Token> pending_tokens{};

    Token lex();

    void advance()
    {
        if (!is_eof())
            position++;
    }

    char peek(int offset = 0) const
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
                advance();
                advance();
                advance();

                while (!is_eof())
                {
                    if (peek() == '-' && peek(1) == '-' && peek(2) == ';')
                    {
                        advance();
                        advance();
                        advance();

                        break;
                    }

                    advance();
                }
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

    bool is_eof() const
    {
        return position >= source.length();
    }
};