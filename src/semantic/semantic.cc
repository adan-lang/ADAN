#include <format>

#include "semantic.hh"

static bool is_numeric_type(const TypePtr &type)
{
    const TypeKind kind = prune(type)->kind;
    return kind == TypeKind::Int || kind == TypeKind::Float;
}

static void require_matching_kinds(const TypePtr &left,
                                   const TypePtr &right,
                                   const std::string &context)
{
    const TypePtr resolved_left = prune(left);
    const TypePtr resolved_right = prune(right);

    if (resolved_left->kind != resolved_right->kind)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("You're trying to use {} with two different types: \"{}\" and \"{}\".",
                                             context,
                                             type_to_string(resolved_left),
                                             type_to_string(resolved_right)));
}

void SemanticAnalyzer::visit(IntLiteral &)
{
    last_expr_type = make_int();
}

void SemanticAnalyzer::visit(FloatLiteral &)
{
    last_expr_type = make_float();
}

void SemanticAnalyzer::visit(BoolLiteral &)
{
    last_expr_type = make_bool();
}

void SemanticAnalyzer::visit(StringLiteral &)
{
    last_expr_type = make_string();
}

void SemanticAnalyzer::visit(InterpolatedString &)
{
    last_expr_type = make_string();
}

void SemanticAnalyzer::visit(Identifier &expr)
{
    Symbol *symbol = table.lookup(expr.name);
    if (!symbol)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Use of undeclared variable \"{}\"", expr.name));
    last_expr_type = symbol->type;
}

void SemanticAnalyzer::visit(ArrayLiteral &expr)
{
    // @todo When the analyzer is improved, make it so it uses unification instead
    if (expr.elements.size() != 0)
    {
        TypePtr array_type;
        for (std::size_t i = 0; i < expr.elements.size(); ++i)
        {
            expr.elements[i]->accept(*this);
            TypePtr current_type = last_expr_type;

            if (!array_type)
            {
                array_type = current_type;
            }
            else if (prune(current_type)->kind != prune(array_type)->kind)
                // @todo @important Design a diagnostics system for debugging during semantic analysis
                throw std::runtime_error(std::format("There's a type mismatch between the array's expected type: \"{}\" and the type specified, which is: \"{}\"",
                                                     type_to_string(prune(current_type)),
                                                     type_to_string(prune(array_type))));
        }

        last_expr_type = make_array(array_type);
    }
    else
    {
        last_expr_type = make_array(make_var());
    }
}

void SemanticAnalyzer::visit(CallExpr &expr)
{
    Symbol *symbol = table.lookup(expr.callee);
    if (!symbol)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Use of undeclared function \"{}\"", expr.callee));

    TypePtr function_type = prune(symbol->type);
    if (function_type->kind == TypeKind::Function)
    {
        if (function_type->params.size() == expr.args.size())
        {
            for (std::size_t i = 0; i < expr.args.size(); ++i)
            {
                expr.args[i]->accept(*this);
                TypePtr argument_type = last_expr_type;

                // @todo When the analyzer is improved, make it so it uses unification instead
                // @important Before unification, this can accept incompatible arguments, such as an array of Int against an array of String
                if (prune(argument_type)->kind == prune(function_type->params[i])->kind)
                {
                    continue;
                }
                else
                {
                    // @todo @important Design a diagnostics system for debugging during semantic analysis
                    throw std::runtime_error(std::format("Argument \"{}\" in the function: \"{}\" does not match its expected type, which is: {}.",
                                                         i + 1,
                                                         expr.callee,
                                                         type_to_string(function_type->params[i])));
                }
            }

            last_expr_type = function_type->result;
        }
        else
        {
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("The arguments specified in function: \"{}\" do not match the parameter's expected amount, which is: {} instead of your specified: {}.",
                                                 expr.callee,
                                                 function_type->params.size(), expr.args.size()));
        }
    }
    else
    {
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("You're attempting to call something that is not a function! In this case, it's: \"{}\"",
                                             symbol->name));
    }
}

void SemanticAnalyzer::visit(BinaryExpr &expr)
{
    const bool is_assignment = expr.op == BinaryOp::Equal ||
                               expr.op == BinaryOp::PlusAssign ||
                               expr.op == BinaryOp::MinusAssign ||
                               expr.op == BinaryOp::MultiplyAssign ||
                               expr.op == BinaryOp::DivideAssign ||
                               expr.op == BinaryOp::ModuloAssign;

    if (is_assignment)
    {
        auto *identifier = dynamic_cast<Identifier *>(expr.lhs.get());
        if (!identifier)
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error("You're trying to assign a value to something that isn't a variable.");

        Symbol *symbol = table.lookup(identifier->name);
        if (!symbol)
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to assign a value to a variable that hasn't been declared: \"{}\"",
                                                 identifier->name));

        if (expr.op != BinaryOp::Equal)
        {
            if (!symbol->initialized)
                // @todo @important Design a diagnostics system for debugging during semantic analysis
                throw std::runtime_error(std::format("You're trying to update a variable before it's been initialized: \"{}\"",
                                                     identifier->name));
            if (!is_numeric_type(symbol->type))
                // @todo @important Design a diagnostics system for debugging during semantic analysis
                throw std::runtime_error(std::format("You're trying to use a compound assignment with a non-number type: \"{}\"",
                                                     type_to_string(symbol->type)));
        }

        expr.rhs->accept(*this);
        TypePtr rhs_type = last_expr_type;
        if (symbol->initialized)
            require_matching_kinds(symbol->type, rhs_type, "Assignment");
        else
            symbol->type = rhs_type;

        if (expr.op == BinaryOp::PlusAssign || expr.op == BinaryOp::MinusAssign ||
            expr.op == BinaryOp::MultiplyAssign || expr.op == BinaryOp::DivideAssign ||
            expr.op == BinaryOp::ModuloAssign)
        {
            if (expr.op == BinaryOp::ModuloAssign && (prune(symbol->type)->kind != TypeKind::Int || prune(rhs_type)->kind != TypeKind::Int))
                // @todo @important Design a diagnostics system for debugging during semantic analysis
                throw std::runtime_error("You're trying to use modulo assignment with something other than Int values.");
            if (!is_numeric_type(rhs_type))
                // @todo @important Design a diagnostics system for debugging during semantic analysis
                throw std::runtime_error(std::format("You're trying to use a compound assignment with a non-number value: {}",
                                                     type_to_string(rhs_type)));
            require_matching_kinds(symbol->type, rhs_type, "Compound assignment");
        }

        symbol->initialized = true;
        last_expr_type = symbol->type;
        return;
    }

    expr.lhs->accept(*this);
    TypePtr lhs_type = last_expr_type;

    expr.rhs->accept(*this);
    TypePtr rhs_type = last_expr_type;

    switch (expr.op)
    {
    case BinaryOp::Plus:
    case BinaryOp::Minus:
    case BinaryOp::Multiply:
    case BinaryOp::Divide:
        require_matching_kinds(lhs_type, rhs_type, "Arithmetic operator");
        if (!is_numeric_type(lhs_type))
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to do arithmetic with a non-number type: {}",
                                                 type_to_string(lhs_type)));
        last_expr_type = lhs_type;
        return;
    case BinaryOp::Modulo:
        require_matching_kinds(lhs_type, rhs_type, "Modulo operator");
        if (prune(lhs_type)->kind != TypeKind::Int)
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to use modulo with a non-Int type: {}",
                                                 type_to_string(lhs_type)));
        last_expr_type = make_int();
        return;
    case BinaryOp::GreaterThan:
    case BinaryOp::GreaterEqual:
    case BinaryOp::LessThan:
    case BinaryOp::LessEqual:
        require_matching_kinds(lhs_type, rhs_type, "Ordered comparison");
        if (!is_numeric_type(lhs_type))
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to compare values by order, but this type isn't numeric: {}",
                                                 type_to_string(lhs_type)));
        last_expr_type = make_bool();
        return;
    case BinaryOp::EqualEqual:
        require_matching_kinds(lhs_type, rhs_type, "Equality comparison");
        last_expr_type = make_bool();
        return;
    case BinaryOp::Equal:
    case BinaryOp::PlusAssign:
    case BinaryOp::MinusAssign:
    case BinaryOp::MultiplyAssign:
    case BinaryOp::DivideAssign:
    case BinaryOp::ModuloAssign:
        break;
    }

    // @todo @important Design a diagnostics system for debugging during semantic analysis
    throw std::runtime_error("You're trying to use a binary operator that ADAN doesn't support yet.");
}

void SemanticAnalyzer::visit(UnaryExpr &expr)
{
    expr.operand->accept(*this);
    TypePtr operand_type = last_expr_type;

    switch (expr.op)
    {
    case UnaryOp::Plus:
    case UnaryOp::Minus:
        if (!is_numeric_type(operand_type))
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to use a unary number operator with a non-number type: {}",
                                                 type_to_string(operand_type)));
        last_expr_type = operand_type;
        return;
    case UnaryOp::Hashtag:
        if (prune(operand_type)->kind != TypeKind::Array)
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to get the length of something that isn't an array: {}",
                                                 type_to_string(operand_type)));
        last_expr_type = make_int();
        return;
    case UnaryOp::Increment:
    case UnaryOp::Decrement:
    {
        auto *identifier = dynamic_cast<Identifier *>(expr.operand.get());
        if (!identifier)
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error("You're trying to increment or decrement something that isn't a variable.");
        Symbol *symbol = table.lookup(identifier->name);
        if (!symbol || !symbol->initialized)
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to increment or decrement a variable that hasn't been initialized: \"{}\"",
                                                 identifier->name));
        if (!is_numeric_type(symbol->type))
            // @todo @important Design a diagnostics system for debugging during semantic analysis
            throw std::runtime_error(std::format("You're trying to increment or decrement a non-number value: {}",
                                                 type_to_string(symbol->type)));
        last_expr_type = symbol->type;
        return;
    }
    case UnaryOp::Dollar:
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error("You're trying to use \"$\", but its type behavior hasn't been defined yet.");
    }
}

void SemanticAnalyzer::visit(ReturnStmt &stmt)
{
    stmt.value->accept(*this);
    TypePtr return_type = last_expr_type;

    // @todo @important After the other statements are complete (for the most part), work on this!!
    // @brief This is gonna be a little bit annoying to get working.
}

void SemanticAnalyzer::visit(ExprStmt &stmt)
{
    stmt.expr->accept(*this);
}

void SemanticAnalyzer::visit(LocalDecl &stmt)
{
    TypePtr local_value_type;
    bool initialized{};

    if (stmt.init)
    {
        stmt.init->accept(*this);
        local_value_type = last_expr_type;
        initialized = true;
    }
    else
    {
        local_value_type = make_var();
    }

    Symbol variable{
        stmt.name,
        local_value_type,
        initialized,
    };

    table.declare(variable);
}

void SemanticAnalyzer::visit(FuncDecl &stmt)
{
}

void SemanticAnalyzer::visit(IfStmt &stmt)
{
}

void SemanticAnalyzer::visit(NumericForStmt &stmt)
{
}

void SemanticAnalyzer::visit(ForInStmt &stmt)
{
}

void SemanticAnalyzer::visit(WhileStmt &stmt)
{
}

void SemanticAnalyzer::visit(Block &block)
{
    table.enter_scope();
    for (auto &stmt : block.stmts)
        stmt->accept(*this);
    table.exit_scope();
}