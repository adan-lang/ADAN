#include <format>

#include "ast-helpers.hh"
#include "ast.hh"
#include "operator-table.hh"
#include "type-checker.hh"
#include "type-helpers.hh"
#include "types.hh"

static bool is_compound_assignment(BinaryOp op)
{
    switch (op)
    {
    case BinaryOp::PlusAssign:
    case BinaryOp::MinusAssign:
    case BinaryOp::MultiplyAssign:
    case BinaryOp::DivideAssign:
    case BinaryOp::ModuloAssign:
        return true;
    default:
        return false;
    }
}

static bool is_mutable_lvalue(Expr &expr)
{
    return dynamic_cast<Identifier *>(&expr) != nullptr || dynamic_cast<ArrayIndex *>(&expr) != nullptr;
}

static bool rebind_compound_target(Expr &expr, TypePtr type, ScopeManager &symbol_table)
{
    if (auto *identifier = dynamic_cast<Identifier *>(&expr))
    {
        Symbol *symbol = symbol_table.lookup(identifier->name);
        if (symbol)
        {
            symbol->type = std::move(type);
            return true;
        }
        return false;
    }

    std::size_t array_depth{};
    Expr *base = &expr;
    while (auto *index = dynamic_cast<ArrayIndex *>(base))
    {
        ++array_depth;
        base = index->array.get();
    }

    auto *identifier = dynamic_cast<Identifier *>(base);
    if (!identifier || array_depth == 0)
        return false;

    Symbol *symbol = symbol_table.lookup(identifier->name);
    if (!symbol)
        return false;

    for (std::size_t i{}; i < array_depth; ++i)
        type = make_array(std::move(type));

    symbol->type = std::move(type);
    return true;
}

static std::vector<BinaryOperator> matching_binary_rules(OperatorTable &table, BinaryOp op, const TypePtr &lhs, const TypePtr &rhs, const TypePtr &result = {})
{
    std::vector<BinaryOperator> matches;
    for (const BinaryOperator &rule : table.get_binary_rules(op))
    {
        if (type_can_match(lhs, rule.lhs) && type_can_match(rhs, rule.rhs) && (!result || type_can_match(result, rule.result)))
            matches.emplace_back(rule);
    }

    return matches;
}

void TypeChecker::visit(ArrayIndex &node)
{
    TypePtr array = infer(*node.array);
    TypePtr array_generic = make_array(make_var());
    unify(array, array_generic, diagnostics);

    TypePtr index = infer(*node.index);
    unify(index, make_int(), diagnostics);

    last_inferred_type = array_generic->element;
}

void TypeChecker::visit(CallExpr &node)
{
    Symbol *callee = symbol_table.lookup(node.callee);
    if (!callee)
        diagnostics.error(std::format("Attempted to call an unknown function: {}", node.callee));

    TypePtr callee_t = prune(callee->type);
    if (callee_t->kind != TypeKind::Function)
        diagnostics.error("Attempted to call something that isn't a function");

    if (node.args.size() != callee_t->params.size())
        diagnostics.error(std::format("Function: \"{}\" expects {} argument(s), but got {}", node.callee, callee_t->params.size(), node.args.size()));

    for (std::size_t i{}; i < callee_t->params.size(); ++i)
    {
        unify(infer(*node.args[i]), callee_t->params[i], diagnostics);
    }

    last_inferred_type = callee_t->result;
}

void TypeChecker::visit(BinaryExpr &node)
{
    TypePtr lhs;
    if (node.op == BinaryOp::Equal && dynamic_cast<Identifier *>(node.lhs.get()))
    {
        const bool previous_use_tracking = count_identifier_uses;
        count_identifier_uses = false;
        lhs = infer(*node.lhs);
        count_identifier_uses = previous_use_tracking;
    }
    else
    {
        lhs = infer(*node.lhs);
    }

    TypePtr rhs = infer(*node.rhs);
    if (node.op == BinaryOp::Equal)
    {
        if (is_mutable_lvalue(*node.lhs))
        {
            unify(lhs, rhs, diagnostics);
            last_inferred_type = lhs;
            return;
        }

        diagnostics.error("The left side of an assignment must be a variable or array index");
    }

    const bool compound_assignment = is_compound_assignment(node.op);
    if (compound_assignment && !is_mutable_lvalue(*node.lhs))
        diagnostics.error("The left side of a compound assignment must be a "
                          "variable or array index");

    const std::vector<BinaryOperator> matches = matching_binary_rules(operator_table, node.op, lhs, rhs);
    if (matches.empty())
    {
        diagnostics.error(std::format("Type mismatch on the binary operator: {}, with the types: {} (left) "
                                      "and {} (right)",
                                      op_to_string(node.op),
                                      type_to_string(lhs),
                                      type_to_string(rhs)));
    }

    if (matches.size() == 1)
    {
        const BinaryOperator &rule = matches.front();

        unify(lhs, rule.lhs, diagnostics);
        unify(rhs, rule.rhs, diagnostics);

        if (compound_assignment)
        {
            const bool target_widenable = rebind_compound_target(*node.lhs, rule.result, symbol_table);
            if (!target_widenable && !(*lhs == *rule.result))
                diagnostics.error("This compound assignment cannot widen its target type");
        }

        last_inferred_type = rule.result;
        return;
    }

    TypePtr result = make_var();

    const bool target_widenable = compound_assignment ? rebind_compound_target(*node.lhs, result, symbol_table) : true;
    pending_binary_operators.push_back(PendingBinaryOperator{
        lhs,
        node.op,
        rhs,
        result,
        target_widenable,
    });

    last_inferred_type = result;
}

void TypeChecker::visit(Identifier &node)
{
    Symbol *symbol = symbol_table.lookup(node.name);
    if (!symbol)
        diagnostics.error(std::format("Unknown identifier: {}", node.name));

    if (count_identifier_uses)
        symbol->used = true;
    last_inferred_type = symbol->type;
}

void TypeChecker::visit(UnaryExpr &node)
{
    TypePtr operand = infer(*node.operand);
    if (node.op == UnaryOp::Hashtag)
    {
        unify(operand, make_array(make_var()), diagnostics);
        last_inferred_type = make_int();
        return;
    }

    std::vector<UnaryOperator> matches;
    for (const UnaryOperator &rule : operator_table.get_unary_rules(node.op))
    {
        if (type_can_match(operand, rule.operand))
            matches.emplace_back(rule);
    }

    if (matches.empty())
        diagnostics.error(std::format("Type mismatch on the unary operator: {}, with the type: {}", op_to_string(node.op), type_to_string(operand)));

    if (matches.size() == 1)
    {
        unify(operand, matches.front().operand, diagnostics);
        last_inferred_type = matches.front().result;
        return;
    }

    TypePtr result = make_var();

    pending_unary_operators.push_back(PendingUnaryOperator{operand, node.op, result});
    last_inferred_type = result;
}