#include <charconv>

#include "parser.hh"

Block Parser::parse_block()
{
}

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
    case TokenKind::Local:
        return parse_local();
    default:
        return std::make_unique<ExprStmt>(parse_expr());
    }
}

// <- <value>
std::unique_ptr<Stmt> Parser::parse_return()
{
    expect(TokenKind::BackArrow);
    auto expression = parse_expr();

    return std::make_unique<ReturnStmt>(std::move(expression));
}

// local <key> = <value>?
std::unique_ptr<Stmt> Parser::parse_local()
{
    auto name_token = expect(TokenKind::Local);
    auto name = std::move(name_token.lexeme);

    std::unique_ptr<Expr> init;
    if (matches(TokenKind::Equal))
    {
        advance();
        init = parse_expr();
    }

    return std::make_unique<LocalDecl>(std::move(name),
                                       std::move(init));
}

// function <key> (-> (<params>, ...))? <block>
std::unique_ptr<Stmt> Parser::parse_function()
{
    expect(TokenKind::Function);

    auto name_token = expect(TokenKind::Ident);
    auto name = std::move(name_token);

    std::vector<std::string> params;
    if (matches(TokenKind::Arrow))
    {
        advance();
        expect(TokenKind::LParen);

        while (!matches(TokenKind::RParen))
        {
            auto param = expect(TokenKind::Ident);
            params.emplace_back(param);

            if (!matches(TokenKind::Comma))
                break;

            advance();
        }

        expect(TokenKind::RParen);
    }

    Block block = parse_block();
    return std::make_unique<FuncDecl>(std::move(name),
                                      std::move(params),
                                      std::move(block));
}

// if (<conditional>)? <block>
std::unique_ptr<Stmt> Parser::parse_if()
{
    expect(TokenKind::If);

    std::unique_ptr<Expr> condition = parse_expr();
    if (matches(TokenKind::LBrace))
        condition = std::make_unique<BoolLiteral>(true);
    else
        condition = parse_expr();

    Block body = parse_block();
    return std::make_unique<IfStmt>(std::move(condition),
                                    std::move(body));
}

// for <key>   =    <start>  <goal>, (<step>)?
// for <name> in <iterable> <block>
std::unique_ptr<Stmt> Parser::parse_for()
{
}

// while (<conditional>)? <block>
std::unique_ptr<Stmt> Parser::parse_while()
{
    expect(TokenKind::While);

    std::unique_ptr<Expr> condition;
    if (matches(TokenKind::LBrace))
        condition = std::make_unique<BoolLiteral>(true);
    else
        condition = parse_expr();

    Block body = parse_block();
    return std::make_unique<WhileStmt>(std::move(condition),
                                       std::move(body));
}