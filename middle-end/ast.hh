#pragma once

#include <optional>
#include  <variant>
#include   <vector>
#include   <string>
#include   <memory>
#include  <cstdint>

struct Expression;

typedef std::unique_ptr<Expression> Expr;

typedef struct IntLiteralNode
{
    int64_t value;
} IntLiteralNode;

typedef struct FloatLiteralNode
{
    double value;
} FloatLiteralNode;

typedef struct StringLiteralNode
{
    std::string value;
} StringLiteralNode;

typedef struct CharLiteralNode
{
    char value;
} CharLiteralNode;

typedef struct BoolLiteralNode
{
    bool value;
} BoolLiteralNode;

typedef struct NoneLiteralNode
{
} NoneLiteralNode;

typedef struct IdentifierNode
{
    std::string name;
} IdentifierNode;

typedef struct BinaryExprNode
{
    Expr left;
    std::string op;
    Expr right;
} BinaryExprNode;

typedef struct UnaryExprNode
{
    std::string op;
    Expr operand;
} UnaryExprNode;

typedef struct CallExprNode
{
    Expr callee;
    std::vector<Expr> arguments;
} CallExprNode;

typedef struct IndexExprNode
{
    Expr object;
    Expr index;
} IndexExprNode;

typedef struct ArrayLiteralNode
{
    std::vector<Expr> elements;
} ArrayLiteralNode;

typedef struct LambdaNode
{
    std::vector<struct ParamNode> params;
    std::vector<struct Statement> body;
} LambdaNode;

struct Expression : std::variant
<
    IntLiteralNode,    StringLiteralNode, CharLiteralNode,
    BoolLiteralNode,   NoneLiteralNode,   IdentifierNode,
    BinaryExprNode,    UnaryExprNode,     CallExprNode,
    IndexExprNode,     ArrayLiteralNode,  LambdaNode,
    FloatLiteralNode
>
{
    using variant::variant;
};

typedef struct TypeNode
{
    std::string name;
    int array_depth;
} TypeNode;

typedef struct ParamNode
{
    std::string name;
    std::optional<TypeNode> type;
    bool is_readonly;
    bool is_variadic;
} ParamNode;

typedef struct VarDeclNode
{
    std::string name;
    std::optional<TypeNode> type;
    Expr value;
    bool is_readonly;
    bool is_global;
} VarDeclNode;

typedef struct AssignNode
{
    Expr target;
    std::string op;
    Expr value;
    bool is_global;
} AssignNode;

typedef struct FuncDeclNode
{
    std::string name;
    std::vector<ParamNode> params;
    std::optional<TypeNode> return_type;
    std::vector<struct Statement> body;
} FuncDeclNode;

typedef struct ReturnNode
{
    std::optional<Expr> value;
} ReturnNode;

typedef struct IfNode
{
    Expr condition;
    std::vector<struct Statement> body;
    std::optional<std::vector<struct Statement>> else_body;
} IfNode;

typedef struct WhileNode
{
    Expr condition;
    std::vector<struct Statement> body;
} WhileNode;

typedef struct ForInNode
{
    std::optional<std::string> key;
    std::optional<std::string> value;
    Expr iterable;
    std::vector<struct Statement> body;
} ForInNode;

typedef struct ForNode
{
    std::optional<std::unique_ptr<Statement>> init;
    std::optional<Expr> condition;
    std::optional<Expr> iterate;
    std::vector<struct Statement> body;
} ForNode;

typedef struct RepeatUntilNode
{
    std::vector<struct Statement> body;
    Expr condition;
} RepeatUntilNode;

typedef struct BreakNode
{
} BreakNode;

typedef struct PassNode
{
} PassNode;

typedef struct IncludeNode
{
    std::string path;
} IncludeNode;

typedef struct UsingNode
{
    std::string module_name;
} UsingNode;

typedef struct ExprStmtNode
{
    Expr expression;
} ExprStmtNode;

typedef struct ExportNode
{
    std::unique_ptr<struct Statement> declaration;
} ExportNode;

struct Statement : std::variant
<
    VarDeclNode, AssignNode,   FuncDeclNode,
    ReturnNode,  IfNode,       WhileNode,
    ForInNode,   ForNode,      RepeatUntilNode,
    BreakNode,   PassNode,     IncludeNode,
    UsingNode,   ExprStmtNode, ExportNode
>
{
    using variant::variant;
};

typedef struct ProgramNode
{
    std::vector<Statement> statements;
} ProgramNode;