#pragma once

#include <format>
#include <stdexcept>
#include <vector>

#include "ast.hh"
#include "lexer.hh"

class Parser
{
public:
    std::vector<std::unique_ptr<Expr>> parse()
    {
        std::vector<std::unique_ptr<Expr>> expressions;
        while (current_token.kind != TokenKind::Eof)
            expressions.push_back(parse_expr());

        return expressions;
    }

private:
    Lexer lexer;
    Token current_token;

    Token advance()
    {
        return std::exchange(current_token, lexer.next());
    }

    Token expect(const TokenKind kind)
    {
        if (current_token.kind != kind)
        {
            // @todo @important do some fancy error bullshit later
            throw std::runtime_error(std::format("Expected {}, got {}",
                                                 token_kind_name(kind),
                                                 token_kind_name(current_token.kind)));
        }

        return advance();
    }

    std::unique_ptr<Expr> parse_expr()
    {
        return parse_additive();
    }

    std::unique_ptr<Expr> parse_additive() {}
    std::unique_ptr<Expr> parse_multiplicative() {}
    std::unique_ptr<Expr> parse_primary()
    {
        switch (current_token.kind)
        {
        case TokenKind::Int:
        {
            int64_t value{};
            auto [ptr, ec] = std::from_chars(
                current_token.lexeme.data(),
                current_token.lexeme.data() + current_token.lexeme.size(),
                value);

            if (ec != std::errc())
            {
                // @todo @important do some fancy error bullshit later
                throw std::runtime_error("Failed to parse integer literal");
            }

            advance();

            return std::make_unique<IntLiteral>(value);
        }

        default:
            // @todo @important do some fancy error bullshit later
            throw std::runtime_error(std::format("Expected primary, got {}",
                                                 token_kind_name(current_token.kind)));
        }
    }
};