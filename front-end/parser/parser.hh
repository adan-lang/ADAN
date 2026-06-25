#pragma once

#include   <vector>
#include   <string>
#include   <memory>
#include <optional>

#include            "../tokens.hh"
#include "../../middle-end/ast.hh"

typedef enum Precedence
{
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
} Precedence;

class Parser
{
private:
    std::vector<Token> tokens;
    int pos;

    Token peek(
        int offset = 0
    ) {

    }

    Token current()
    {

    }
    
    Token consume()
    {

    }
    
    Token expect(
        TokenType type
    ) {

    }
    
    bool check(
        TokenType type
    ) {

    }
    
    bool match(
        TokenType type
    ) {

    }
    
    bool is_at_end()
    {

    }

    bool is_assign_op(
        TokenType type
    ) {

    }

    Precedence get_precedence(
        TokenType type
    ) {

    }

    Statement parse_statement()
    {

    }
    
    VarDeclNode parse_variable_decl(
        bool is_readonly,
        bool is_global
    ) {

    }
    
    FuncDeclNode parse_function_decl()
    {

    }
    
    ReturnNode parse_return()
    {

    }
    
    IfNode parse_if_stmt()
    {

    }
    
    WhileNode parse_while_loop()
    {

    }
    
    ForInNode parse_for_in_loop()
    {

    }
    
    ForNode parse_for_loop()
    {

    }
    
    RepeatUntilNode parse_repeat_until_loop()
    {

    }
    
    IncludeNode parse_include()
    {

    }
    
    UsingNode parse_using()
    {

    }
    
    ExportNode parse_export()
    {

    }

    std::vector<Statement> parse_block()
    {

    }
    
    std::vector<ParamNode> parse_params()
    {

    }
    
    std::optional<TypeNode> parse_type_annotation()
    {

    }

    Expr parse_expression(
        Precedence min_prec = PREC_NONE
    ) {

    }

    Expr parse_prefix()
    {

    }

    Expr parse_infix(
        Expr left,
        Token op
    ) {

    }

    Expr parse_call_expr(
        Expr callee
    ) {

    }

    Expr parse_index_expr(
        Expr object
    ) {

    }

    Expr parse_primary()
    {
        
    }

public:
    Parser(
        std::vector<Token> tokens
    );

    ~Parser();
    
    ProgramNode parse();
};