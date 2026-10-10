#include <format>

#include "semantic.hh"

void SemanticAnalyzer::visit(IntLiteral &)
{
    last_expr_type = make_int();
}

void SemanticAnalyzer::visit(FloatLiteral &)
{
    last_expr_type = make_float();
}

void SemanticAnalyzer::visit(BoolLiteral &)
{
    last_expr_type = make_bool();
}

void SemanticAnalyzer::visit(StringLiteral &)
{
    last_expr_type = make_string();
}

void SemanticAnalyzer::visit(InterpolatedString &)
{
    last_expr_type = make_string();
}

void SemanticAnalyzer::visit(Identifier &expr)
{
    Symbol *symbol = table.lookup(expr.name);
    if (!symbol)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Use of undeclared variable \"{}\"", expr.name));
    last_expr_type = symbol->type;
}

void SemanticAnalyzer::visit(ArrayLiteral &)
{
    last_expr_type = make_array(); // @todo do soon
}

void SemanticAnalyzer::visit(CallExpr &expr)
{
    Symbol *symbol = table.lookup(expr.callee);
    if (!symbol)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Use of undeclared function \"{}\"", expr.callee));

    TypePtr function_type = prune(symbol->type);
    if (function_type->kind == TypeKind::Function)
    {
        if (function_type->params.size() == expr.args.size())
        {
            for (std::size_t i = 0; i < expr.args.size(); ++i)
            {
                expr.args[i]->accept(*this);
                TypePtr argument_type = last_expr_type;

                // @todo When the analyzer is improved, make it so it uses unification instead
                // @important Before unification, this can accept incompatible arguments, such as an array of Int against an array of String
                if (prune(argument_type)->kind == prune(function_type->params[i])->kind)
                {
                    continue;
                }
                else
                {
                    // @todo @important Design a diagnostics system for debugging during semantic analysis
                    throw std::runtime_error(std::format("Argument \"{}\" in the function: \"{}\" does not match its expected type, which is: {}.",
                                                         i + 1,
                                                         expr.callee,
                                                         type_to_string(function_type->params[i])));
                }
            }

            last_expr_type = function_type->result;
        }
        else
        {
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("The arguments specified in function: \"{}\" do not match the parameter's expected amount, which is: {} instead of your specified: {}.",
                                                 expr.callee,
                                                 function_type->params.size(), expr.args.size()));
        }
    }
    else
    {
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("You're attempting to call something that is not a function! In this case, it's: \"{}\"",
                                             symbol->name));
    }
}

void SemanticAnalyzer::visit(BinaryExpr &)
{
}

void SemanticAnalyzer::visit(UnaryExpr &)
{
}

void SemanticAnalyzer::visit(ReturnStmt &)
{
}

void SemanticAnalyzer::visit(ExprStmt &)
{
}

void SemanticAnalyzer::visit(LocalDecl &)
{
}

void SemanticAnalyzer::visit(FuncDecl &)
{
}

void SemanticAnalyzer::visit(IfStmt &)
{
}

void SemanticAnalyzer::visit(NumericForStmt &)
{
}

void SemanticAnalyzer::visit(ForInStmt &)
{
}

void SemanticAnalyzer::visit(WhileStmt &)
{
}

void SemanticAnalyzer::visit(Block &block)
{
    table.enter_scope();
    for (auto &stmt : block.stmts)
        stmt->accept(*this);
    table.exit_scope();
}