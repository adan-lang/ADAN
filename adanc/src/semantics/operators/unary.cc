#include "operator-table.hh"
#include "ast.hh"
#include "types.hh"

void register_unary(OperatorTable &table)
{
    {
        table.add_operator(UnaryOperator{make_int(), UnaryOp::Minus, make_int()});
        table.add_operator(UnaryOperator{make_float(), UnaryOp::Minus, make_float()});
    }
    {
        table.add_operator(UnaryOperator{make_bool(), UnaryOp::Not, make_bool()});
    }
    {
        // TEMPORARY UNTIL WE CAN GET NESTING SUPPORT
        
        table.add_operator(UnaryOperator{make_array(make_int()), UnaryOp::Hashtag, make_int()});
        table.add_operator(UnaryOperator{make_array(make_float()), UnaryOp::Hashtag, make_int()});
        table.add_operator(UnaryOperator{make_array(make_bool()), UnaryOp::Hashtag, make_int()});
        table.add_operator(UnaryOperator{make_array(make_string()), UnaryOp::Hashtag, make_int()});
    }
    {
        table.add_operator(UnaryOperator{make_int(), UnaryOp::Increment, make_int()});
        table.add_operator(UnaryOperator{make_float(), UnaryOp::Increment, make_float()});
    }
    {
        table.add_operator(UnaryOperator{make_int(), UnaryOp::Decrement, make_int()});
        table.add_operator(UnaryOperator{make_float(), UnaryOp::Decrement, make_float()});
    }
}