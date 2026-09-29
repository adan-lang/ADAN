#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <variant>

struct Expr
{
    virtual ~Expr() = default;
};

struct IntLiteral : Expr
{
    int64_t value{};
    explicit IntLiteral(const int64_t v) : value{v} {}
};

struct FloatLiteral : Expr
{
    double value{};
    explicit FloatLiteral(const double v) : value{v} {}
};

struct BoolLiteral : Expr
{
    bool value{};
    explicit BoolLiteral(const bool v) : value{v} {}
};

struct StringLiteral : Expr
{
    std::string value;
    explicit StringLiteral(std::string v) : value{std::move(v)} {}
};

// `${x} was interpolated!` -> parts: [Expr(x), " was interpolated!"]
struct InterpolatedString : Expr
{
    using Part = std::variant<std::string, std::unique_ptr<Expr>>;
    std::vector<Part> parts;

    explicit InterpolatedString(std::vector<Part> parts)
        : parts{std::move(parts)} {}
};

struct Identifier : Expr
{
    std::string name;
    explicit Identifier(std::string n) : name{std::move(n)} {}
};

// [<value>, ...]
struct ArrayLiteral : Expr
{
    std::vector<std::unique_ptr<Expr>> elements;

    explicit ArrayLiteral(std::vector<std::unique_ptr<Expr>> elements)
        : elements{std::move(elements)} {}
};

// <key> <- (<args>)
struct CallExpr : Expr
{
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;

    CallExpr(std::string callee, std::vector<std::unique_ptr<Expr>> args)
        : callee{std::move(callee)},
          args{std::move(args)} {}
};

enum class BinaryOp : uint8_t
{
    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,
    GreaterThan,
    GreaterEqual,
    LessThan,
    LessEqual,
    Equal,
    EqualEqual,
    PlusAssign,
    MinusAssign,
    MultiplyAssign,
    DivideAssign,
    ModuloAssign,
};

enum class UnaryOp : uint8_t
{
    Plus,
    Minus,
    Hashtag,
    Dollar,
    Increment,
    Decrement,
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

struct UnaryExpr : Expr
{
    UnaryOp op{};
    bool postfix = false; // @details postfix is for increment/decrement for X++/X--
    std::unique_ptr<Expr> operand;

    UnaryExpr(UnaryOp op, std::unique_ptr<Expr> operand, bool postfix = false)
        : op{op},
          postfix{postfix},
          operand{std::move(operand)} {}
};

struct Stmt
{
    virtual ~Stmt() = default;
};

struct Block
{
    std::vector<std::unique_ptr<Stmt>> stmts;
};

// <- <value>
struct ReturnStmt : Stmt
{
    std::unique_ptr<Expr> value; // @important never gonna be null

    explicit ReturnStmt(std::unique_ptr<Expr> value)
        : value{std::move(value)} {}
};

// an expression used as a statement: calls, `x += 1`, `x++`
struct ExprStmt : Stmt
{
    std::unique_ptr<Expr> expr;

    explicit ExprStmt(std::unique_ptr<Expr> expr)
        : expr{std::move(expr)} {}
};

// local <key> = <value>?
struct LocalDecl : Stmt
{
    std::string name;
    std::unique_ptr<Expr> init;

    LocalDecl(std::string name, std::unique_ptr<Expr> init)
        : name{std::move(name)},
          init{std::move(init)} {}
};

// function <key> (-> (<params>, ...))? <block>
// function <key>  -> (<params>)        { ... }
struct FuncDecl : Stmt
{
    std::string name;
    std::vector<std::string> params;
    Block body;

    FuncDecl(std::string name,
             std::vector<std::string> params,
             Block body)
        : name{std::move(name)},
          params{std::move(params)},
          body{std::move(body)} {}
};

// if (<conditional>)? <block>
// if  <conditional>   { ... }
struct IfStmt : Stmt
{
    std::unique_ptr<Expr> condition;
    Block body;

    IfStmt(std::unique_ptr<Expr> condition,
           Block body)
        : condition{std::move(condition)},
          body{std::move(body)} {}
};

// for <key> = <start>, <goal>, (<step>)?
struct NumericForStmt : Stmt
{
    std::string name;
    std::unique_ptr<Expr> start, stop;
    std::unique_ptr<Expr> step;
    Block body;

    NumericForStmt(std::string name,
                   std::unique_ptr<Expr> start,
                   std::unique_ptr<Expr> stop,
                   std::unique_ptr<Expr> step,
                   Block body)
        : name{std::move(name)},
          start{std::move(start)},
          stop{std::move(stop)},
          step{std::move(step)},
          body{std::move(body)} {}
};

// for <name> in <iterable> <block>
struct ForInStmt : Stmt
{
    std::string var;
    std::unique_ptr<Expr> iterable;
    Block body;

    ForInStmt(std::string var, std::unique_ptr<Expr> iterable, Block body)
        : var{std::move(var)},
          iterable{std::move(iterable)},
          body{std::move(body)} {}
};

// while (<conditional>)? <block>
// while  <conditional>   { ... }
struct WhileStmt : Stmt
{
    std::unique_ptr<Expr> condition;
    Block body;

    WhileStmt(std::unique_ptr<Expr> condition,
              Block body)
        : condition{std::move(condition)},
          body{std::move(body)} {}
};