#pragma once

#include <cstdint>
#include <memory>

struct Expr
{
    virtual ~Expr() = default;
};

struct Literal : Expr
{
    int64_t value{};

    explicit Literal(const int64_t v) : value{v} {}
};

enum class BinaryOp : uint8_t
{
    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,
    GreaterThan,
    LessThan,
    Equal,
};

enum class UnaryOp : uint8_t
{
    Hashtag,
};

struct BinaryExpr : Expr
{
    BinaryOp op{};
    std::unique_ptr<Expr> lhs, rhs;

    BinaryExpr(BinaryOp op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : op{op},
          lhs{std::move(lhs)},
          rhs{std::move(rhs)} {}
};