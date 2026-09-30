#include <charconv>

#include "parser.hh"

// expressions

std::unique_ptr<Expr> Parser::parse_expr()
{
    return parse_additive();
}

std::unique_ptr<Expr> Parser::parse_additive()
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

std::unique_ptr<Expr> Parser::parse_multiplicative()
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

std::unique_ptr<Expr> Parser::parse_primary()
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

// statements

std::unique_ptr<Stmt> Parser::parse_stmt()
{
    switch (current_token.kind)
    {
        // @todo @important make cases for stmts

    case TokenKind::BackArrow:
        return parse_return();
    default:
        return std::make_unique<ExprStmt>(parse_expr());
    }
}

std::unique_ptr<Stmt> Parser::parse_return()
{
    expect(TokenKind::BackArrow);

    auto expression = parse_expr();
    return std::make_unique<ReturnStmt>(std::move(expression));
}

std::unique_ptr<Stmt> Parser::parse_local()
{
    expect(TokenKind::Local);

    std::string name =
}