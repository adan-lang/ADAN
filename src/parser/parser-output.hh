#pragma once

#include <format>
#include <string>
#include <string_view>

#include "ast.hh"

inline std::string binary_operator_name(const BinaryOp op)
{
    switch (op)
    {
    case BinaryOp::Plus:
        return "+";
    case BinaryOp::Minus:
        return "-";
    case BinaryOp::Multiply:
        return "*";
    case BinaryOp::Divide:
        return "/";
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

    return "?";
}

inline std::string unary_operator_name(const UnaryOp op)
{
    switch (op)
    {
    case UnaryOp::Plus:
        return "+";
    case UnaryOp::Minus:
        return "-";
    case UnaryOp::Hashtag:
        return "#";
    case UnaryOp::Dollar:
        return "$";
    case UnaryOp::Increment:
        return "++";
    case UnaryOp::Decrement:
        return "--";
    }

    return "?";
}

inline std::string format_expression(const Expr &expression)
{
    if (const auto *literal = dynamic_cast<const IntLiteral *>(&expression))
        return std::format("{}", literal->value);
    if (const auto *literal = dynamic_cast<const FloatLiteral *>(&expression))
        return std::format("{}", literal->value);
    if (const auto *literal = dynamic_cast<const BoolLiteral *>(&expression))
        return literal->value ? "true" : "false";
    if (const auto *literal = dynamic_cast<const StringLiteral *>(&expression))
        return std::format("\"{}\"", literal->value);
    if (const auto *identifier = dynamic_cast<const Identifier *>(&expression))
        return identifier->name;
    if (const auto *binary = dynamic_cast<const BinaryExpr *>(&expression))
        return std::format("({} {} {})", format_expression(*binary->lhs),
                           binary_operator_name(binary->op), format_expression(*binary->rhs));

    if (const auto *unary = dynamic_cast<const UnaryExpr *>(&expression))
    {
        const auto operand = format_expression(*unary->operand);
        const auto op = unary_operator_name(unary->op);

        return unary->postfix ? std::format("({}{})", operand, op)
                              : std::format("({}{})", op, operand);
    }

    if (const auto *array = dynamic_cast<const ArrayLiteral *>(&expression))
    {
        std::string result = "[";

        for (std::size_t index = 0; index < array->elements.size(); ++index)
        {
            if (index != 0)
                result += ", ";

            result += format_expression(*array->elements[index]);
        }

        return result + "]";
    }

    if (const auto *call = dynamic_cast<const CallExpr *>(&expression))
    {
        std::string result = call->callee + "(";

        for (std::size_t index = 0; index < call->args.size(); ++index)
        {
            if (index != 0)
                result += ", ";

            result += format_expression(*call->args[index]);
        }

        return result + ")";
    }

    if (const auto *interpolated = dynamic_cast<const InterpolatedString *>(&expression))
    {
        std::string result = "`";

        for (const auto &part : interpolated->parts)
        {
            if (const auto *text = std::get_if<std::string>(&part))
                result += *text;
            else
                result += std::format("${{{}}}", format_expression(**std::get_if<std::unique_ptr<Expr>>(&part)));
        }

        return result + "`";
    }

    return "<Unknown Expression>";
}

inline std::string format_parser_output(const std::vector<std::unique_ptr<Expr>> &expressions)
{
    std::string result = "Parser output:";
    for (std::size_t index = 0; index < expressions.size(); ++index)
        result += std::format("\n  {}: {}", index + 1, format_expression(*expressions[index]));

    return result;
}