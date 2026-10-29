#include <unordered_map>
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
    std::unordered_map<BinaryOp, std::vector<BinaryOperator>> binary_operators;
    std::unordered_map<UnaryOp, std::vector<UnaryOperator>> unary_operators;

public:
    OperatorTable() = default;

    void add_operator(BinaryOp op);

    BinaryOp get_binary_operators();
    UnaryOp get_unary_operators();
};