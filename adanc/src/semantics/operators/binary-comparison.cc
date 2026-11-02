#include "operator-table.hh"
#include "ast.hh"

void register_binary_comparison(OperatorTable &table)
{
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterThan, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterThan, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterThan, make_float(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterThan, make_float(), make_bool()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterEqual, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterEqual, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterEqual, make_float(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterEqual, make_float(), make_bool()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::LessThan, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::LessThan, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::LessThan, make_float(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::LessThan, make_float(), make_bool()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::LessEqual, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::LessEqual, make_int(), make_bool()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::LessEqual, make_float(), make_bool()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::LessEqual, make_float(), make_bool()});
    }
}