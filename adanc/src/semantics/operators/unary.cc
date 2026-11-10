#include "ast.hh"
#include "operator-table.hh"
#include "types.hh"

void register_unary(OperatorTable &table) {
  {
    table.add_operator(UnaryOperator{make_int(), UnaryOp::Minus, make_int()});
    table.add_operator(
        UnaryOperator{make_float(), UnaryOp::Minus, make_float()});
  }
  {
    table.add_operator(UnaryOperator{make_bool(), UnaryOp::Not, make_bool()});
  }
  {
    table.add_operator(
        UnaryOperator{make_int(), UnaryOp::Increment, make_int()});
    table.add_operator(
        UnaryOperator{make_float(), UnaryOp::Increment, make_float()});
  }
  {
    table.add_operator(
        UnaryOperator{make_int(), UnaryOp::Decrement, make_int()});
    table.add_operator(
        UnaryOperator{make_float(), UnaryOp::Decrement, make_float()});
  }
}