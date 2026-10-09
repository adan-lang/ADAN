#include <charconv>

#include "parser.hh"

Block Parser::parse_block()
{
    expect(TokenKind::LBrace);
    Block block;

    while (!matches(TokenKind::RBrace))
    {
        if (matches(TokenKind::Eof))
        {
            // @todo @important Design a diagnostics system for debugging during parsing
            throw std::runtime_error("Unterminated block: Expected '}'");
        }

        block.stmts.push_back(parse_stmt());
    }

    expect(TokenKind::RBrace);
    return block;
}

// expressions

std::unique_ptr<Expr> Parser::parse_expr()
{
    auto lhs = parse_comparison();

    BinaryOp op;
    switch (current_token.kind)
    {
    case TokenKind::Equal:
        op = BinaryOp::Equal;
        break;
    case TokenKind::PlusAssign:
        op = BinaryOp::PlusAssign;
        break;
    case TokenKind::MinusAssign:
        op = BinaryOp::MinusAssign;
        break;
    case TokenKind::MultiplyAssign:
        op = BinaryOp::MultiplyAssign;
        break;
    case TokenKind::DivideAssign:
        op = BinaryOp::DivideAssign;
        break;
    case TokenKind::ModuloAssign:
        op = BinaryOp::ModuloAssign;
        break;
    default:
        return lhs;
    }

    advance();
    auto rhs = parse_expr();
    return std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
}

std::unique_ptr<Expr> Parser::parse_comparison()
{
    auto lhs = parse_additive();

    while (matches(TokenKind::GreaterThan, TokenKind::GreaterEqual,
                   TokenKind::LessThan, TokenKind::LessEqual,
                   TokenKind::EqualEqual))
    {
        BinaryOp op;
        switch (current_token.kind)
        {
        case TokenKind::GreaterThan:
            op = BinaryOp::GreaterThan;
            break;
        case TokenKind::GreaterEqual:
            op = BinaryOp::GreaterEqual;
            break;
        case TokenKind::LessThan:
            op = BinaryOp::LessThan;
            break;
        case TokenKind::LessEqual:
            op = BinaryOp::LessEqual;
            break;
        default:
            op = BinaryOp::EqualEqual;
            break;
        }

        advance();
        auto rhs = parse_additive();
        lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }

    return lhs;
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
    auto lhs = parse_unary();

    while (matches(TokenKind::Multiply, TokenKind::Divide, TokenKind::Modulo))
    {
        BinaryOp op;
        switch (current_token.kind)
        {
        case TokenKind::Multiply:
            op = BinaryOp::Multiply;
            break;
        case TokenKind::Divide:
            op = BinaryOp::Divide;
            break;
        default:
            op = BinaryOp::Modulo;
            break;
        }
        advance();

        auto rhs = parse_unary();
        lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::parse_unary()
{
    UnaryOp op;
    switch (current_token.kind)
    {
    case TokenKind::Plus:
        op = UnaryOp::Plus;
        break;
    case TokenKind::Minus:
        op = UnaryOp::Minus;
        break;
    case TokenKind::Hashtag:
        op = UnaryOp::Hashtag;
        break;
    case TokenKind::Dollar:
        op = UnaryOp::Dollar;
        break;
    case TokenKind::Increment:
        op = UnaryOp::Increment;
        break;
    case TokenKind::Decrement:
        op = UnaryOp::Decrement;
        break;
    default:
    {
        auto expression = parse_primary();
        while (matches(TokenKind::Increment, TokenKind::Decrement))
        {
            op = current_token.kind == TokenKind::Increment
                     ? UnaryOp::Increment
                     : UnaryOp::Decrement;
            advance();
            expression = std::make_unique<UnaryExpr>(op, std::move(expression), true);
        }
        return expression;
    }
    }

    advance();
    auto operand = parse_unary();
    return std::make_unique<UnaryExpr>(op, std::move(operand));
}

std::unique_ptr<Expr> Parser::parse_array_literal()
{
    expect(TokenKind::LBracket);
    std::vector<std::unique_ptr<Expr>> elements;

    while (!matches(TokenKind::RBracket))
    {
        if (matches(TokenKind::Eof))
            throw std::runtime_error("Unterminated array literal: expected ']' ");

        elements.push_back(parse_expr());

        if (!matches(TokenKind::Comma))
            break;

        advance();
        if (matches(TokenKind::RBracket))
            break;
    }

    expect(TokenKind::RBracket);
    return std::make_unique<ArrayLiteral>(std::move(elements));
}

std::unique_ptr<Expr> Parser::parse_template_string()
{
    std::vector<InterpolatedString::Part> parts;

    while (matches(TokenKind::TemplateString, TokenKind::InterpolateStart))
    {
        if (matches(TokenKind::TemplateString))
        {
            auto segment = advance();
            parts.emplace_back(std::move(segment.lexeme));
            continue;
        }

        advance();
        auto expression = parse_expr();
        expect(TokenKind::InterpolateEnd);
        parts.emplace_back(std::move(expression));
    }

    return std::make_unique<InterpolatedString>(std::move(parts));
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
            // @todo @important Design a diagnostics system for debugging during parsing
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
            // @todo @important Design a diagnostics system for debugging during parsing
            throw std::runtime_error("Failed to parse floating-point literal");
        }

        advance();
        return std::make_unique<FloatLiteral>(value);
    }

    case TokenKind::String:
    {
        auto token = advance();
        return std::make_unique<StringLiteral>(std::move(token.lexeme));
    }

    case TokenKind::Bool:
    {
        auto token = advance();
        return std::make_unique<BoolLiteral>(token.lexeme == "true");
    }

    case TokenKind::TemplateString:
    case TokenKind::InterpolateStart:
        return parse_template_string();

    case TokenKind::LBracket:
        return parse_array_literal();

    case TokenKind::Ident:
    {
        auto token = advance();
        if (!matches(TokenKind::BackArrow))
            return std::make_unique<Identifier>(std::move(token.lexeme));

        advance();
        expect(TokenKind::LParen);
        std::vector<std::unique_ptr<Expr>> args;
        while (!matches(TokenKind::RParen))
        {
            if (matches(TokenKind::Eof))
                throw std::runtime_error("Unterminated function call: expected ')'");

            args.push_back(parse_expr());
            if (!matches(TokenKind::Comma))
                break;

            advance();
            if (matches(TokenKind::RParen))
                break;
        }

        expect(TokenKind::RParen);
        return std::make_unique<CallExpr>(std::move(token.lexeme), std::move(args));
    }

    case TokenKind::LParen:
    {
        advance();
        auto expression = parse_expr();
        expect(TokenKind::RParen);

        return expression;
    }

    default:
        // @todo @important Design a diagnostics system for debugging during parsing
        throw std::runtime_error(std::format("Expected primary, got {}",
                                             token_kind_name(current_token.kind)));
    }
}

// statements

std::unique_ptr<Stmt> Parser::parse_stmt()
{
    switch (current_token.kind)
    {
    case TokenKind::BackArrow:
        return parse_return();
    case TokenKind::Local:
        return parse_local();
    case TokenKind::Function:
        return parse_function();
    case TokenKind::If:
        return parse_if();
    case TokenKind::For:
        return parse_for();
    case TokenKind::While:
        return parse_while();
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
    expect(TokenKind::Local);

    auto name_token = expect(TokenKind::Ident);
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
    auto name = std::move(name_token.lexeme);

    std::vector<std::string> params;
    if (matches(TokenKind::Arrow))
    {
        advance();
        expect(TokenKind::LParen);

        while (!matches(TokenKind::RParen))
        {
            auto param = expect(TokenKind::Ident);
            params.emplace_back(param.lexeme);

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

    std::unique_ptr<Expr> condition;
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
    expect(TokenKind::For);

    auto name_token = expect(TokenKind::Ident);
    auto name = std::move(name_token.lexeme);

    if (matches(TokenKind::In))
    {
        advance();
        auto iterable = parse_expr();
        Block body = parse_block();
        return std::make_unique<ForInStmt>(std::move(name),
                                           std::move(iterable),
                                           std::move(body));
    }

    if (matches(TokenKind::Equal))
    {
        advance();
        auto start = parse_expr();
        expect(TokenKind::Comma);
        auto stop = parse_expr();

        std::unique_ptr<Expr> step = std::make_unique<IntLiteral>(1);
        if (matches(TokenKind::Comma))
        {
            advance();
            step = parse_expr();
        }

        Block body = parse_block();
        return std::make_unique<NumericForStmt>(std::move(name),
                                                std::move(start),
                                                std::move(stop),
                                                std::move(step),
                                                std::move(body));
    }

    // @todo @important Design a diagnostics system for debugging during parsing
    throw std::runtime_error("Expected 'in' or '=' after for-loop variable");
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