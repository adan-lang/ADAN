#pragma once

#include <format>
#include <stdexcept>
#include <vector>

#include "ast.hh"
#include "lexer.hh"

class Parser
{
public:
    explicit Parser(std::string_view input) : lexer{input}
    {
        current_token = lexer.next();
    }

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

    template <typename... T>
    bool matches(T... types) const
    {
        return ((current_token.kind == types) || ...);
    }

    std::unique_ptr<Expr> parse_expr()
    {
        return parse_additive();
    }

    std::unique_ptr<Stmt> parse_stmt()
    {
        switch (current_token.kind)
        {
            // @todo @important make cases for stmts

        default:
            return std::make_unique<ExprStmt>(parse_expr());
        }
    }

    std::unique_ptr<Expr> parse_additive()
    {
        auto lhs = parse_multiplicative();

        while (matches(TokenKind::Plus, TokenKind::Minus))
        {
            auto op = current_token.kind == TokenKind::Plus ? BinaryOp::Plus : BinaryOp::Minus;
            advance();

            auto rhs = parse_multiplicative();
            lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
        }

        return lhs;
    }

    std::unique_ptr<Expr> parse_multiplicative()
    {
        auto lhs = parse_primary();

        while (matches(TokenKind::Multiply, TokenKind::Divide))
        {
            auto op = current_token.kind == TokenKind::Multiply ? BinaryOp::Multiply : BinaryOp::Divide;
            advance();

            auto rhs = parse_primary();
            lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
        }

        return lhs;
    }

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

        case TokenKind::Float:
        {
            double value{};
            auto [ptr, ec] = std::from_chars(
                current_token.lexeme.data(),
                current_token.lexeme.data() + current_token.lexeme.size(),
                value);

            if (ec != std::errc())
            {
                // @todo @important do some fancy error bullshit later
                throw std::runtime_error("Failed to parse floating-point literal");
            }

            advance();
            return std::make_unique<FloatLiteral>(value);
        }

        case TokenKind::Ident:
        {
            auto token = advance();
            return std::make_unique<Identifier>(std::move(token.lexeme));
        }

        case TokenKind::LParen:
        {
            advance();
            auto expression = parse_expr();
            expect(TokenKind::RParen);
            return expression;
        }

        default:
            // @todo @important do some fancy error bullshit later
            throw std::runtime_error(std::format("Expected primary, got {}",
                                                 token_kind_name(current_token.kind)));
        }
    }
};