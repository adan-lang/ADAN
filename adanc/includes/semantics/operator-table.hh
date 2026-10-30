#include <vector>

#include "ast.hh"
#include "types.hh"

struct BinaryOperator
{
    TypePtr lhs;
    BinaryOp op;
    TypePtr rhs;
};

struct UnaryOperator
{
    TypePtr _type;
    UnaryOp op;
};

class OperatorTable
{
    std::vector<BinaryOperator> binary_operators;
    std::vector<UnaryOperator> unary_operators;

public:
    OperatorTable() = default;

    inline void add_operator(BinaryOperator op)
    {
        binary_operators.emplace_back(op);
    }

    inline void add_operator(UnaryOperator op)
    {
        unary_operators.emplace_back(op);
    }

    inline std::vector<BinaryOperator> get_binary_operators()
    {
        return binary_operators;
    }

    inline std::vector<UnaryOperator> get_unary_operators()
    {
        return unary_operators;
    }
};