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

    Block parse_block();

    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Expr> parse_additive();
    std::unique_ptr<Expr> parse_multiplicative();
    std::unique_ptr<Expr> parse_primary();

    std::unique_ptr<Stmt> parse_stmt();
    std::unique_ptr<Stmt> parse_return();
    std::unique_ptr<Stmt> parse_local();
    std::unique_ptr<Stmt> parse_function();
    std::unique_ptr<Stmt> parse_if();
    std::unique_ptr<Stmt> parse_for();
    std::unique_ptr<Stmt> parse_while();
};