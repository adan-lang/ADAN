#include "ast.hh"
#include "operator-table.hh"

void register_binary_assignment(OperatorTable &table)
{
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::PlusAssign, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::PlusAssign, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::PlusAssign, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::PlusAssign, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_string(), BinaryOp::PlusAssign, make_string(), make_string()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::MinusAssign, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::MinusAssign, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::MinusAssign, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::MinusAssign, make_float(), make_float()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::MultiplyAssign, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::MultiplyAssign, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::MultiplyAssign, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::MultiplyAssign, make_float(), make_float()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::DivideAssign, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::DivideAssign, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::DivideAssign, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::DivideAssign, make_float(), make_float()});
    }
    {
        table.add_operator(BinaryOperator{make_int(), BinaryOp::ModuloAssign, make_int(), make_int()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::ModuloAssign, make_int(), make_float()});
        table.add_operator(BinaryOperator{make_int(), BinaryOp::ModuloAssign, make_float(), make_float()});
        table.add_operator(BinaryOperator{make_float(), BinaryOp::ModuloAssign, make_float(), make_float()});
    }
}