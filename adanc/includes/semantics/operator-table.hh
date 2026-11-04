#pragma once

#include <vector>

#include "ast.hh"
#include "types.hh"

struct BinaryOperator
{
    TypePtr lhs;
    BinaryOp op;
    TypePtr rhs;

    TypePtr result;
};

struct UnaryOperator
{
    TypePtr operand;
    UnaryOp op;

    TypePtr result;
};

class OperatorTable
{
    std::vector<BinaryOperator> binary_operators;
    std::vector<UnaryOperator> unary_operators;

public:
    OperatorTable();

    // Binary Operators

    inline void add_operator(BinaryOperator op)
    {
        binary_operators.emplace_back(op);
    }

    inline std::vector<BinaryOperator> get_binary_operators()
    {
        return binary_operators;
    }

    /**
     *
     * @note A "rule" is just another way of saying a `BinaryOperator` object.
     *
     * `Int + Int -> Int` is a rule, which refers to:
     * ```c++
     * BinaryOperator{
     *     make_int(),      // LHS Type
     *     BinaryOp::Plus,  // Operator
     *     make_int(),      // RHS Type
     *     make_int(),      // Result Type
     * };
     * ```
     */
    inline std::vector<BinaryOperator> get_binary_rules(BinaryOp op)
    {
        std::vector<BinaryOperator> rules;

        for (std::size_t i{}; i < binary_operators.size(); ++i)
        {
            if (binary_operators[i].op == op)
                rules.emplace_back(binary_operators[i]);
        }

        return rules;
    }

    // Unary Operators

    inline void add_operator(UnaryOperator op)
    {
        unary_operators.emplace_back(op);
    }

    inline std::vector<UnaryOperator> get_unary_operators()
    {
        return unary_operators;
    }

    inline std::vector<UnaryOperator> get_unary_rules(UnaryOp op)
    {
        std::vector<UnaryOperator> rules;

        for (std::size_t i{}; i < unary_operators.size(); ++i)
        {
            if (unary_operators[i].op == op)
                rules.emplace_back(unary_operators[i]);
        }

        return rules;
    }
};