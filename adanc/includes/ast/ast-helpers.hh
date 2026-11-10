#include "ast.hh"

inline std::string_view op_to_string(BinaryOp op) {
  switch (op) {
  case BinaryOp::Plus:
    return "+";
  case BinaryOp::Minus:
    return "-";
  case BinaryOp::Multiply:
    return "*";
  case BinaryOp::Divide:
    return "/";
  case BinaryOp::Power:
    return "^";
  case BinaryOp::Modulo:
    return "%";
  case BinaryOp::GreaterThan:
    return ">";
  case BinaryOp::GreaterEqual:
    return ">=";
  case BinaryOp::LessThan:
    return "<";
  case BinaryOp::LessEqual:
    return "<=";
  case BinaryOp::Equal:
    return "=";
  case BinaryOp::EqualEqual:
    return "==";
  case BinaryOp::NotEqualEqual:
    return "!==";
  case BinaryOp::PlusAssign:
    return "+=";
  case BinaryOp::MinusAssign:
    return "-=";
  case BinaryOp::MultiplyAssign:
    return "*=";
  case BinaryOp::DivideAssign:
    return "/=";
  case BinaryOp::ModuloAssign:
    return "%=";
  }

  return "[??]";
}

inline std::string_view op_to_string(UnaryOp op) {
  switch (op) {
  case UnaryOp::Minus:
    return "-";
  case UnaryOp::Not:
    return "!";
  case UnaryOp::Hashtag:
    return "#";
  case UnaryOp::Increment:
    return "++";
  case UnaryOp::Decrement:
    return "--";
  }

  return "[??]";
}