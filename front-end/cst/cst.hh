#pragma once

#include <variant>
#include <string>
#include <memory>
#include <vector>

#include "tokens.hh"

struct CST;
using NodePtr = std::unique_ptr<CST>;

// quick note: terminals refer to tokens the lexer directly produces
//             non-terminals are constructs that're defined in the language's grammar rules

//// TERMINALS ////

struct Literal
{
    enum class Kind
    {
        Int,
        String,
        Bool
    } kind;
    std::string raw_text;
    int line;
    int col;

    // accessors

    int as_int() const
    {
        return std::stoi(raw_text);
    }

    bool as_bool() const
    {
        return raw_text == "true";
    }

    std::string as_string() const
    {
        return raw_text;
    }
};

struct Identifier
{
    std::string name;
    int line;
    int col;
};

/// KEYWORDS ///

struct VarKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct GlobalKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct ReadonlyKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct IfKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct WhileKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct ForKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct RepeatKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct ExportKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct IncludeKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct TodoKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct PassKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct BreakKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct ReturnKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct UntilKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct FuncKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct LambdaKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct UsingKeyword
{
    std::string raw_text;
    int line;
    int col;
};

struct ModuleKeyword
{
    std::string raw_text;
    int line;
    int col;
};

/// TYPES ///

struct StringType
{
    std::string raw_text;
    int line;
    int col;
};

struct CharType
{
    std::string raw_text;
    int line;
    int col;
};

struct NumberType
{
    std::string raw_text;
    int line;
    int col;
};

struct IntType
{
    std::string raw_text;
    int line;
    int col;
};

struct FloatType
{
    std::string raw_text;
    int line;
    int col;
};

struct BoolType
{
    std::string raw_text;
    int line;
    int col;
};

struct NoneType
{
    std::string raw_text;
    int line;
    int col;
};

struct VoidType
{
    std::string raw_text;
    int line;
    int col;
};

struct AutoType
{
    std::string raw_text;
    int line;
    int col;
};

//// NON-TERMINALS ////

struct TypeAnnotation
{
    Token base_type;                // basically tells you if it's an Int, String, Char, whatever
    bool is_optional;               // trailing ?
    std::vector<int> array_dim;     // 0 = not an array, 1 = [], 2 = [][], etc.
    int line;
    int col;
};

struct FuncParameter // reminder: does not refer to the function itself, but the arguments of it!!
{
    Token identifier;
    bool is_readonly;
    bool is_variadic;
    TypeAnnotation* type;
    int line;
    int col;
};

struct VarDeclaration
{
    VarKeyword keyword;
    bool is_global;
    bool is_readonly;
    Token identifier;
    TypeAnnotation* type;
    NodePtr init;
    Token semicolon;
    int line;
    int col;
};

struct FuncDeclaration
{
    FuncKeyword keyword;
    Token identifier;
    Token open_paren;
    std::vector<FuncParameter> parameters;
    Token close_paren;
    TypeAnnotation* return_type;    // if left unfilled, infer it instead
    NodePtr body;
    int line;
    int col;
};

struct LambdaDeclaration
{
    LambdaKeyword keyword;
    Token open_paren;
    std::vector<FuncParameter> parameters;
    Token close_paren;
    NodePtr body;
    int line;
    int col;
};

struct IfStatement
{
    IfKeyword keyword;
    Token open_paren_opt;    // clear indicator parenthesis are optional! :3
    NodePtr condition;
    Token close_paren_opt;
    NodePtr then_body;
    Token else_keyword_opt;
    NodePtr else_body;
    int line;
    int col;
};

struct WhileStatement
{
    WhileKeyword keyword;
    Token open_paren_opt;
    NodePtr condition;
    Token close_paren_opt;
    NodePtr body;
    int line;
    int col;
};

struct ForInLoop
{
    ForKeyword keyword;
    Token key_opt;          // optional loop variable (the key)
    Token value;            // primarily loop variable (or the only one if key is left blank)
    Token in;
    NodePtr iterable;
    NodePtr body;
    int line;
    int col;
};

struct ForCLoop // c-styled
{
    ForKeyword keyword;
    Token semicolon1;
    NodePtr init_opt;
    Token semicolon2;
    NodePtr condition_opt;
    Token semicolon3;
    NodePtr iterate_opt;
    NodePtr body;
    int line;
    int col;
};

struct RepeatUntilLoop
{
    RepeatKeyword keyword;
    NodePtr body;
    UntilKeyword until;
    NodePtr condition;
    Token semicolon;
    int line;
    int col;
};

struct BreakStatement
{
    BreakKeyword keyword;
    Token semicolon;
    int line;
    int col;
};

struct PassStatement
{
    PassKeyword keyword;
    Token semicolon;
    int line;
    int col;
};

struct ReturnStatement
{
    ReturnKeyword keyword;
    NodePtr value_opt;
    Token semicolon;
    int line;
    int col;
}; 

struct FuncCall
{
    Token identifier;
    Token open_paren;
    std::vector<NodePtr> arguments;
    Token close_paren;
    int line;
    int col;
};

struct ArrayLiteral
{
    Token open_bracket;
    std::vector<NodePtr> elements;
    Token close_bracket;
    int line;
    int col;
};

struct ArrayAccess
{
    NodePtr array;
    Token open_bracket;
    NodePtr index;
    Token close_bracket;
    int line;
    int col;
};

struct Assignment // =, +=, -=, *=, etc. (any of these and so on)
{
    NodePtr target;
    Token op;
    NodePtr value;
    int line;
    int col;
};

struct NotNoneAssertion
{
    NodePtr operand;
    Token op;
    int line;
    int col;
};

struct IncludeStatement
{
    IncludeKeyword keyword;
    Token file_path; // std::string
    Token semicolon;
    int line;
    int col;
};

struct UsingModuleStatement
{
    UsingKeyword keyword1;
    ModuleKeyword keyword2;
    Token module_path;      // an example is like "std.io"
    Token semicolon;
    int line;
    int col;
};

struct ExportDeclaration
{
    ExportKeyword keyword;
    NodePtr declaration;
    int line;
    int col;
};

struct TodoExpression
{
    TodoKeyword keyword;
    int line;
    int col;
};

struct BinaryOp
{
    Token op;
    NodePtr lhs;
    NodePtr rhs;
    int line;
    int col;
};

struct UnaryOp
{
    Token op;
    NodePtr operand;
    int line;
    int col;
};

struct TernaryOp
{
    NodePtr condition;
    Token question;
    NodePtr true_expr;
    Token colon;
    NodePtr false_expr;
    int line;
    int col;
};

struct ElvisOp
{
    NodePtr lhs;
    Token elvis_op;
    NodePtr rhs;
    int line;
    int col;
};

struct Block
{
    Token open_brace;
    std::vector<NodePtr> statements;
    Token close_brace;
    int line;
    int col;
};

struct Program
{
    std::vector<NodePtr> top_level;     // top-level decls and statements (scope 0)
    int line;
    int col;
};

//// CST /////

struct CST
{
    std::variant<
        Literal,
        Identifier,

        VarKeyword,
        GlobalKeyword,
        ReadonlyKeyword,
        IfKeyword,
        WhileKeyword,
        ForKeyword,
        RepeatKeyword,
        ExportKeyword,
        IncludeKeyword,
        TodoKeyword,
        UsingKeyword,
        ModuleKeyword,
        PassKeyword,
        BreakKeyword,
        ReturnKeyword,
        UntilKeyword,
        FuncKeyword,
        LambdaKeyword,
        
        StringType,
        CharType,
        NumberType,
        IntType,
        FloatType,
        BoolType,
        NoneType,
        VoidType,
        AutoType,

        VarDeclaration,
        FuncDeclaration,
        FuncParameter,
        LambdaDeclaration,

        IfStatement,
        WhileStatement,
        ForInLoop,
        ForCLoop,
        RepeatUntilLoop,
        BreakStatement,
        PassStatement,
        ReturnStatement,

        BinaryOp,
        UnaryOp,
        FuncCall,
        ArrayLiteral,
        ArrayAccess,
        Assignment,
        TernaryOp,
        ElvisOp,
        NotNoneAssertion,

        Block,
        Program,

        IncludeStatement,
        UsingModuleStatement,
        ExportDeclaration,

        TypeAnnotation,
        TodoExpression
    > data;
};