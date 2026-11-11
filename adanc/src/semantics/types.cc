#include <format>
#include <stdexcept>
#include <utility>
#include <vector>

#include "diagnostics.hh"
#include "type-helpers.hh"

TypePtr make_int()
{
    return make_type(TypeKind::Int);
}

TypePtr make_float()
{
    return make_type(TypeKind::Float);
}

TypePtr make_bool()
{
    return make_type(TypeKind::Bool);
}

TypePtr make_string()
{
    return make_type(TypeKind::String);
}

TypePtr make_void()
{
    return make_type(TypeKind::Void);
}

TypePtr make_array(TypePtr element)
{
    auto array_type = make_type(TypeKind::Array);
    array_type->element = std::move(element);
    return array_type;
}

TypePtr make_function(std::vector<TypePtr> params, TypePtr result)
{
    auto function_type = make_type(TypeKind::Function);
    function_type->params = std::move(params);
    function_type->result = std::move(result);
    return function_type;
}

TypePtr make_var()
{
    static int next_id = 1;
    auto var_type = make_type(TypeKind::Var);
    var_type->id = next_id++;
    return var_type;
}

/**
 *
 * @brief `prune` works for Var and recursively calls itself until it reaches
 * the end, then returns the type
 */
TypePtr prune(const TypePtr &type)
{
    if (type->kind == TypeKind::Var && type->link)
    {
        type->link = prune(type->link);
        return type->link;
    }

    return type;
}

/**
 *
 * @brief The intentions of `unify` are to equalize the two given types.
 *
 * Unification either confirms two types agree, learns what an unknown
 *  type must be, or reports that it's impossible for them to agree.
 */
void unify(TypePtr a, TypePtr b, Diagnostics &diagnostics)
{
    auto lhs = prune(a);
    auto rhs = prune(b);

    if (lhs == rhs)
        return;

    if (lhs->kind == TypeKind::Var)
    {
        if (occurs(lhs, rhs))
            diagnostics.error("The left type occurs inside the right type");

        lhs->link = rhs;
        return;
    }

    if (rhs->kind == TypeKind::Var)
    {
        if (occurs(rhs, lhs))
            diagnostics.error("The right type occurs inside the left type");

        rhs->link = lhs;
        return;
    }

    if (lhs->kind != rhs->kind)
        diagnostics.error(std::format("There was a mismatch between: (left) {} and (right) {}", type_to_string(lhs), type_to_string(rhs)));

    switch (lhs->kind)
    {
    case TypeKind::Int:
    case TypeKind::Float:
    case TypeKind::Bool:
    case TypeKind::String:
    case TypeKind::Void:
        return;

    case TypeKind::Array:
        unify(lhs->element, rhs->element, diagnostics);
        return;

    case TypeKind::Function:
        if (lhs->params.size() != rhs->params.size())
            diagnostics.error("Function parameter count mismatch");

        for (std::size_t i{}; i < lhs->params.size(); ++i)
            unify(lhs->params[i], rhs->params[i], diagnostics);

        unify(lhs->result, rhs->result, diagnostics);
        return;

    // Should just be impossible to get here
    case TypeKind::Var:
        throw std::logic_error("An unresolved variable has reached the structural comparison stage.");
    }
}