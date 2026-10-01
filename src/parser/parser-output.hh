// made using AI

// this file was made using Generative AI! the contents of the file
//  you're viewing is used for visualizing the AST and nothing more.

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

inline std::string tree_child_prefix(const std::string &prefix, const bool is_last)
{
    return prefix + (is_last ? "    " : "|   ");
}

inline void append_tree_line(std::string &output,
                             const std::string &prefix,
                             const bool is_last,
                             const std::string &label)
{
    output += "\n" + prefix + (is_last ? "`-- " : "|-- ") + label;
}

inline void append_expression_tree(std::string &output,
                                   const Expr &expression,
                                   const std::string &prefix,
                                   const bool is_last);

inline void append_expression_branch(std::string &output,
                                     const std::string &prefix,
                                     const bool is_last,
                                     const std::string &label,
                                     const Expr &expression)
{
    append_tree_line(output, prefix, is_last, label);
    append_expression_tree(output, expression, tree_child_prefix(prefix, is_last), true);
}

inline void append_expression_tree(std::string &output,
                                   const Expr &expression,
                                   const std::string &prefix,
                                   const bool is_last)
{
    if (const auto *literal = dynamic_cast<const IntLiteral *>(&expression))
        append_tree_line(output, prefix, is_last, std::format("IntLiteral: {}", literal->value));
    else if (const auto *literal = dynamic_cast<const FloatLiteral *>(&expression))
        append_tree_line(output, prefix, is_last, std::format("FloatLiteral: {}", literal->value));
    else if (const auto *literal = dynamic_cast<const BoolLiteral *>(&expression))
        append_tree_line(output, prefix, is_last,
                         literal->value ? "BoolLiteral: true" : "BoolLiteral: false");
    else if (const auto *literal = dynamic_cast<const StringLiteral *>(&expression))
        append_tree_line(output, prefix, is_last, std::format("StringLiteral: \"{}\"", literal->value));
    else if (const auto *identifier = dynamic_cast<const Identifier *>(&expression))
        append_tree_line(output, prefix, is_last, std::format("Identifier: {}", identifier->name));
    else if (const auto *binary = dynamic_cast<const BinaryExpr *>(&expression))
    {
        append_tree_line(output, prefix, is_last,
                         std::format("BinaryExpr: {}", binary_operator_name(binary->op)));
        const auto children = tree_child_prefix(prefix, is_last);
        append_expression_branch(output, children, false, "Left", *binary->lhs);
        append_expression_branch(output, children, true, "Right", *binary->rhs);
    }
    else if (const auto *unary = dynamic_cast<const UnaryExpr *>(&expression))
    {
        append_tree_line(output, prefix, is_last,
                         std::format("UnaryExpr: {}{}", unary_operator_name(unary->op),
                                     unary->postfix ? " (postfix)" : " (prefix)"));
        append_expression_branch(output, tree_child_prefix(prefix, is_last), true,
                                 "Operand", *unary->operand);
    }
    else if (const auto *array = dynamic_cast<const ArrayLiteral *>(&expression))
    {
        append_tree_line(output, prefix, is_last, "ArrayLiteral");
        const auto children = tree_child_prefix(prefix, is_last);
        for (std::size_t index = 0; index < array->elements.size(); ++index)
            append_expression_branch(output, children, index + 1 == array->elements.size(),
                                     std::format("Element {}", index), *array->elements[index]);
    }
    else if (const auto *call = dynamic_cast<const CallExpr *>(&expression))
    {
        append_tree_line(output, prefix, is_last, std::format("CallExpr: {}", call->callee));
        const auto children = tree_child_prefix(prefix, is_last);
        for (std::size_t index = 0; index < call->args.size(); ++index)
            append_expression_branch(output, children, index + 1 == call->args.size(),
                                     std::format("Argument {}", index + 1), *call->args[index]);
    }
    else if (const auto *interpolated = dynamic_cast<const InterpolatedString *>(&expression))
    {
        append_tree_line(output, prefix, is_last, "InterpolatedString");
        const auto children = tree_child_prefix(prefix, is_last);
        for (std::size_t index = 0; index < interpolated->parts.size(); ++index)
        {
            const bool part_is_last = index + 1 == interpolated->parts.size();
            const auto &part = interpolated->parts[index];
            if (const auto *text = std::get_if<std::string>(&part))
                append_tree_line(output, children, part_is_last, std::format("Text: \"{}\"", *text));
            else
                append_expression_branch(output, children, part_is_last, "Interpolation",
                                         **std::get_if<std::unique_ptr<Expr>>(&part));
        }
    }
    else
        append_tree_line(output, prefix, is_last, "Unknown Expression");
}

inline void append_statement_tree(std::string &output,
                                  const Stmt &statement,
                                  const std::string &prefix,
                                  const bool is_last);

inline void append_block_tree(std::string &output,
                              const Block &block,
                              const std::string &prefix,
                              const bool is_last)
{
    append_tree_line(output, prefix, is_last, "Block");
    const auto children = tree_child_prefix(prefix, is_last);
    for (std::size_t index = 0; index < block.stmts.size(); ++index)
        append_statement_tree(output, *block.stmts[index], children,
                              index + 1 == block.stmts.size());
}

inline void append_block_branch(std::string &output,
                                const std::string &prefix,
                                const bool is_last,
                                const std::string &label,
                                const Block &block)
{
    append_tree_line(output, prefix, is_last, label);
    append_block_tree(output, block, tree_child_prefix(prefix, is_last), true);
}

inline void append_statement_tree(std::string &output,
                                  const Stmt &statement,
                                  const std::string &prefix,
                                  const bool is_last)
{
    if (const auto *local = dynamic_cast<const LocalDecl *>(&statement))
    {
        append_tree_line(output, prefix, is_last, std::format("LocalDecl: {}", local->name));
        if (local->init)
            append_expression_branch(output, tree_child_prefix(prefix, is_last), true,
                                     "Initializer", *local->init);
    }
    else if (const auto *return_stmt = dynamic_cast<const ReturnStmt *>(&statement))
    {
        append_tree_line(output, prefix, is_last, "ReturnStmt");
        append_expression_branch(output, tree_child_prefix(prefix, is_last), true,
                                 "Value", *return_stmt->value);
    }
    else if (const auto *expression_stmt = dynamic_cast<const ExprStmt *>(&statement))
    {
        append_tree_line(output, prefix, is_last, "ExprStmt");
        append_expression_branch(output, tree_child_prefix(prefix, is_last), true,
                                 "Expression", *expression_stmt->expr);
    }
    else if (const auto *function = dynamic_cast<const FuncDecl *>(&statement))
    {
        append_tree_line(output, prefix, is_last, std::format("FuncDecl: {}", function->name));
        const auto children = tree_child_prefix(prefix, is_last);
        const std::size_t child_count = function->params.size() + 1;
        for (std::size_t index = 0; index < function->params.size(); ++index)
            append_tree_line(output, children, index + 1 == child_count,
                             std::format("Parameter: {}", function->params[index]));
        append_block_branch(output, children, true, "Body", function->body);
    }
    else if (const auto *if_stmt = dynamic_cast<const IfStmt *>(&statement))
    {
        append_tree_line(output, prefix, is_last, "IfStmt");
        const auto children = tree_child_prefix(prefix, is_last);
        append_expression_branch(output, children, false, "Condition", *if_stmt->condition);
        append_block_branch(output, children, true, "Body", if_stmt->body);
    }
    else if (const auto *for_in = dynamic_cast<const ForInStmt *>(&statement))
    {
        append_tree_line(output, prefix, is_last, std::format("ForInStmt: {}", for_in->var));
        const auto children = tree_child_prefix(prefix, is_last);
        append_expression_branch(output, children, false, "Iterable", *for_in->iterable);
        append_block_branch(output, children, true, "Body", for_in->body);
    }
    else if (const auto *numeric_for = dynamic_cast<const NumericForStmt *>(&statement))
    {
        append_tree_line(output, prefix, is_last, std::format("NumericForStmt: {}", numeric_for->name));
        const auto children = tree_child_prefix(prefix, is_last);
        append_expression_branch(output, children, false, "Start", *numeric_for->start);
        append_expression_branch(output, children, false, "Stop", *numeric_for->stop);
        append_expression_branch(output, children, false, "Step", *numeric_for->step);
        append_block_branch(output, children, true, "Body", numeric_for->body);
    }
    else if (const auto *while_stmt = dynamic_cast<const WhileStmt *>(&statement))
    {
        append_tree_line(output, prefix, is_last, "WhileStmt");
        const auto children = tree_child_prefix(prefix, is_last);
        append_expression_branch(output, children, false, "Condition", *while_stmt->condition);
        append_block_branch(output, children, true, "Body", while_stmt->body);
    }
    else
        append_tree_line(output, prefix, is_last, "Unknown Statement");
}

inline std::string format_parser_output(const std::vector<std::unique_ptr<Stmt>> &statements)
{
    std::string result = "Program";
    for (std::size_t index = 0; index < statements.size(); ++index)
        append_statement_tree(result, *statements[index], "", index + 1 == statements.size());
    return result;
}