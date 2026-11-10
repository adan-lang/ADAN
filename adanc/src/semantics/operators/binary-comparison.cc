#include "ast.hh"
#include "operator-table.hh"
#include "types.hh"

void register_binary_comparison(OperatorTable &table) {
  {
    table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterThan,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterThan,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterThan,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterThan,
                                      make_float(), make_bool()});
  }
  {
    table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_int(), BinaryOp::GreaterEqual,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::GreaterEqual,
                                      make_float(), make_bool()});
  }
  {
    table.add_operator(BinaryOperator{make_int(), BinaryOp::LessThan,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::LessThan,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_int(), BinaryOp::LessThan,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::LessThan,
                                      make_float(), make_bool()});
  }
  {
    table.add_operator(BinaryOperator{make_int(), BinaryOp::LessEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::LessEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_int(), BinaryOp::LessEqual,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::LessEqual,
                                      make_float(), make_bool()});
  }
  {
    table.add_operator(BinaryOperator{make_int(), BinaryOp::EqualEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::EqualEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_int(), BinaryOp::EqualEqual,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::EqualEqual,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_bool(), BinaryOp::EqualEqual,
                                      make_bool(), make_bool()});
    table.add_operator(BinaryOperator{make_string(), BinaryOp::EqualEqual,
                                      make_string(), make_bool()});
  }
  {
    table.add_operator(BinaryOperator{make_int(), BinaryOp::NotEqualEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::NotEqualEqual,
                                      make_int(), make_bool()});
    table.add_operator(BinaryOperator{make_int(), BinaryOp::NotEqualEqual,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_float(), BinaryOp::NotEqualEqual,
                                      make_float(), make_bool()});
    table.add_operator(BinaryOperator{make_bool(), BinaryOp::NotEqualEqual,
                                      make_bool(), make_bool()});
    table.add_operator(BinaryOperator{make_string(), BinaryOp::NotEqualEqual,
                                      make_string(), make_bool()});
  }
}