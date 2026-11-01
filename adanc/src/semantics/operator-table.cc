#include <unordered_map>
#include <vector>

#include "operator-table.hh"

namespace
{
    std::vector<BinaryOperator> numeric_rules(BinaryOp op, TypePtr int_result, TypePtr float_result, bool allow_conversion)
    {
        std::vector<BinaryOperator> rules{
            BinaryOperator{
                .lhs = make_int(),
                .op = op,
                .rhs = make_int(),
                .result = int_result,
            },
            BinaryOperator{
                .lhs = make_float(),
                .op = op,
                .rhs = make_int(),
                .result = float_result,
            },
            BinaryOperator{
                .lhs = make_float(),
                .op = op,
                .rhs = make_float(),
                .result = float_result,
            },
        };

        if (allow_conversion)
            rules.emplace_back(BinaryOperator{
                .lhs = make_int(),
                .op = op,
                .rhs = make_float(),
                .result = float_result,
            });

        return rules;
    }

    std::vector<BinaryOperator> same_type_rules(BinaryOp op)
    {
        return {
            BinaryOperator{.lhs = make_int(), .op = op, .rhs = make_int(), .result = make_int()},
            BinaryOperator{.lhs = make_float(), .op = op, .rhs = make_float(), .result = make_float()},
            BinaryOperator{.lhs = make_bool(), .op = op, .rhs = make_bool(), .result = make_bool()},
            BinaryOperator{.lhs = make_string(), .op = op, .rhs = make_string(), .result = make_string()},
        };
    }

    std::vector<BinaryOperator> equality_rules()
    {
        std::vector<BinaryOperator> rules = numeric_rules(BinaryOp::EqualEqual, make_bool(), make_bool(), true);
        rules.emplace_back(BinaryOperator{.lhs = make_bool(), .op = BinaryOp::EqualEqual, .rhs = make_bool(), .result = make_bool()});
        rules.emplace_back(BinaryOperator{.lhs = make_string(), .op = BinaryOp::EqualEqual, .rhs = make_string(), .result = make_bool()});
        return rules;
    }

    std::vector<UnaryOperator> numeric_unary_rules(UnaryOp op)
    {
        return {
            UnaryOperator{.operand = make_int(), .op = op, .result = make_int()},
            UnaryOperator{.operand = make_float(), .op = op, .result = make_float()},
        };
    }

    const std::unordered_map<BinaryOp, std::vector<BinaryOperator>> binary_operator_rules = {
        {
            BinaryOp::Plus,
            numeric_rules(BinaryOp::Plus, make_int(), make_float(), true),
        },
        {
            BinaryOp::Minus,
            numeric_rules(BinaryOp::Minus, make_int(), make_float(), true),
        },
        {
            BinaryOp::Multiply,
            numeric_rules(BinaryOp::Multiply, make_int(), make_float(), true),
        },
        {
            BinaryOp::Divide,
            numeric_rules(BinaryOp::Divide, make_int(), make_float(), true),
        },
        {
            BinaryOp::Power,
            numeric_rules(BinaryOp::Power, make_int(), make_float(), true),
        },
        {
            BinaryOp::Modulo,
            numeric_rules(BinaryOp::Modulo, make_int(), make_float(), true),
        },
        {
            BinaryOp::GreaterThan,
            numeric_rules(BinaryOp::GreaterThan, make_bool(), make_bool(), true),
        },
        {
            BinaryOp::GreaterEqual,
            numeric_rules(BinaryOp::GreaterEqual, make_bool(), make_bool(), true),
        },
        {
            BinaryOp::LessThan,
            numeric_rules(BinaryOp::LessThan, make_bool(), make_bool(), true),
        },
        {
            BinaryOp::LessEqual,
            numeric_rules(BinaryOp::LessEqual, make_bool(), make_bool(), true),
        },
        {
            BinaryOp::Equal,
            same_type_rules(BinaryOp::Equal),
        },
        {
            BinaryOp::EqualEqual,
            equality_rules(),
        },
        {
            BinaryOp::PlusAssign,
            numeric_rules(BinaryOp::PlusAssign, make_int(), make_float(), false),
        },
        {
            BinaryOp::MinusAssign,
            numeric_rules(BinaryOp::MinusAssign, make_int(), make_float(), false),
        },
        {
            BinaryOp::MultiplyAssign,
            numeric_rules(BinaryOp::MultiplyAssign, make_int(), make_float(), false),
        },
        {
            BinaryOp::DivideAssign,
            numeric_rules(BinaryOp::DivideAssign, make_int(), make_float(), false),
        },
        {
            BinaryOp::ModuloAssign,
            numeric_rules(BinaryOp::ModuloAssign, make_int(), make_float(), false),
        },
    };

    const std::unordered_map<UnaryOp, std::vector<UnaryOperator>> unary_operator_rules = {
        {UnaryOp::Minus, numeric_unary_rules(UnaryOp::Minus)},
        {UnaryOp::Increment, numeric_unary_rules(UnaryOp::Increment)},
        {UnaryOp::Decrement, numeric_unary_rules(UnaryOp::Decrement)},
    };
}

OperatorTable::OperatorTable()
{
    for (const auto &entry : binary_operator_rules)
        for (const BinaryOperator &rule : entry.second)
            add_operator(rule);

    for (const auto &entry : unary_operator_rules)
        for (const UnaryOperator &rule : entry.second)
            add_operator(rule);
}