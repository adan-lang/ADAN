#include "ast.hh"
#include "operator-table.hh"

void register_binary_arithmetic(OperatorTable &table)
{
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Plus, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Plus, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Plus, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Plus, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_string(), BinaryOp::Plus, make_string(), make_string()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Minus, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Minus, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Minus, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Minus, make_float(), make_float()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Multiply, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Multiply, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Multiply, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Multiply, make_float(), make_float()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Divide, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Divide, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Divide, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Divide, make_float(), make_float()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Power, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Power, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Power, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Power, make_float(), make_float()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Modulo, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Modulo, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::Modulo, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::Modulo, make_float(), make_float()});
    }
}