#include "ast.hh"
#include "declaration-collector.hh"
#include "type-checker.hh"
#include "types.hh"

void TypeChecker::visit(ReturnStmt &node) {
  if (!curr_func_result_type)
    diagnostics.error("Return statement outside a function");

  const TypePtr return_type = node.value ? infer(*node.value) : make_void();
  unify(curr_func_result_type, return_type, diagnostics);
}

void TypeChecker::visit(ExprStmt &node) { TypePtr expr = infer(*node.expr); }

void TypeChecker::visit(IfStmt &node) {
  unify(infer(*node.condition), make_bool(), diagnostics);

  symbol_table.enter_scope();

  try {
    DeclarationCollector collector{symbol_table};
    for (const auto &stmt : node.body.stmts)
      stmt->accept(collector);

    check(node.body);
  } catch (...) {
    symbol_table.exit_scope();
    throw;
  }

  symbol_table.exit_scope();
}

void TypeChecker::visit(NumericForStmt &node) {
  TypePtr start = infer(*node.start);
  TypePtr stop = infer(*node.stop);
  TypePtr step = infer(*node.step);

  unify(start, make_int(), diagnostics);
  unify(stop, make_int(), diagnostics);
  unify(step, make_int(), diagnostics);

  symbol_table.enter_scope();

  try {
    symbol_table.declare(Symbol{node.name, make_int(), SymbolKind::Local});

    DeclarationCollector collector{symbol_table};
    for (const auto &stmt : node.body.stmts)
      stmt->accept(collector);

    check(node.body);
  } catch (...) {
    symbol_table.exit_scope();
    throw;
  }

  symbol_table.exit_scope();
}

void TypeChecker::visit(ForInStmt &node) {
  TypePtr array = infer(*node.array);
  TypePtr array_generic = make_array(make_var());

  unify(array, array_generic, diagnostics);
  symbol_table.enter_scope();

  try {
    symbol_table.declare(
        Symbol{node.var, array_generic->element, SymbolKind::Local});

    DeclarationCollector collector{symbol_table};
    for (const auto &stmt : node.body.stmts)
      stmt->accept(collector);

    check(node.body);
  } catch (...) {
    symbol_table.exit_scope();
    throw;
  }

  symbol_table.exit_scope();
}

void TypeChecker::visit(WhileStmt &node) {
  TypePtr condition = infer(*node.condition);

  unify(condition, make_bool(), diagnostics);
  symbol_table.enter_scope();

  try {
    DeclarationCollector collector{symbol_table};
    for (const auto &stmt : node.body.stmts)
      stmt->accept(collector);

    check(node.body);
  } catch (...) {
    symbol_table.exit_scope();
    throw;
  }

  symbol_table.exit_scope();
}

void TypeChecker::visit(LocalDecl &node) {
  if (!node.init)
    return;

  Symbol *symbol = symbol_table.lookup(node.name);
  if (!symbol)
    diagnostics.error("Local declaration was not collected");

  unify(symbol->type, infer(*node.init), diagnostics);
}

void TypeChecker::visit(FuncDecl &node) {
  Symbol *symbol = symbol_table.lookup(node.name);
  if (!symbol)
    diagnostics.error("Function declaration was not collected");

  const TypePtr function_type = prune(symbol->type);
  if (function_type->kind != TypeKind::Function)
    diagnostics.error("Function name does not have a function type");
  if (function_type->params.size() != node.params.size())
    diagnostics.error("Function parameter count does not match its type");

  const TypePtr prev_func_result_type = curr_func_result_type;
  symbol_table.enter_scope();

  try {
    for (std::size_t i{}; i < node.params.size(); ++i)
      symbol_table.declare(Symbol{node.params[i], function_type->params[i],
                                  SymbolKind::Parameter});

    DeclarationCollector collector{symbol_table};
    for (const auto &stmt : node.body.stmts)
      stmt->accept(collector);

    curr_func_result_type = function_type->result;
    check(node.body);
  } catch (...) {
    curr_func_result_type = prev_func_result_type;
    symbol_table.exit_scope();
    throw;
  }

  curr_func_result_type = prev_func_result_type;
  symbol_table.exit_scope();
}