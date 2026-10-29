#include <stdexcept>

#include "declaration-collector.hh"
#include "type-checker.hh"

void TypeChecker::visit(ReturnStmt &node)
{
    if (!curr_func_result_type)
        throw std::runtime_error("Return statement outside a function");

    const TypePtr return_type = node.value ? infer(*node.value) : make_void();
    unify(curr_func_result_type, return_type);
}

void TypeChecker::visit(LocalDecl &node)
{
    if (!node.init)
        return;

    Symbol *symbol = symbol_table.lookup(node.name);
    if (!symbol)
        throw std::runtime_error("Local declaration was not collected");

    unify(symbol->type, infer(*node.init));
}

void TypeChecker::visit(FuncDecl &node)
{
    Symbol *symbol = symbol_table.lookup(node.name);
    if (!symbol)
        throw std::runtime_error("Function declaration was not collected");

    const TypePtr function_type = prune(symbol->type);
    if (function_type->kind != TypeKind::Function)
        throw std::runtime_error("Function name does not have a function type");
    if (function_type->params.size() != node.params.size())
        throw std::runtime_error("Function parameter count does not match its type");

    const TypePtr prev_func_result_type = curr_func_result_type;
    symbol_table.enter_scope();

    try
    {
        for (std::size_t i{}; i < node.params.size(); ++i)
            symbol_table.declare(Symbol{node.params[i], function_type->params[i]});

        DeclarationCollector collector{symbol_table};
        for (const auto &stmt : node.body.stmts)
            stmt->accept(collector);

        curr_func_result_type = function_type->result;
        check(node.body);
    }
    catch (...)
    {
        curr_func_result_type = prev_func_result_type;
        symbol_table.exit_scope();
        throw;
    }

    curr_func_result_type = prev_func_result_type;
    symbol_table.exit_scope();
}