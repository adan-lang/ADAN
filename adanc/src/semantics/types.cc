#include <format>
#include <stdexcept>

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
 * @brief `prune` works for Var and recursively calls itself until it reaches the end, then returns the type
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

void unify(TypePtr a, TypePtr b)
{
    auto lhs = prune(a);
    auto rhs = prune(b);

    if (lhs == rhs)
        return;

    if (lhs->kind == TypeKind::Var)
    {
        lhs->link = rhs;
        return;
    }

    if (rhs->kind == TypeKind::Var)
    {
        rhs->link = lhs;
        return;
    }

    if (lhs->kind != rhs->kind)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("There was a mismatch between: (left) {} and (right) {}", type_to_string(lhs), type_to_string(rhs)));

    switch (lhs->kind)
    {
    case TypeKind::Int:
    case TypeKind::Float:
    case TypeKind::Bool:
    case TypeKind::String:
    case TypeKind::Void:
        return;

    // @todo Work on these when you get type inference
    case TypeKind::Array:
        unify(lhs->element, rhs->element);

    case TypeKind::Function:
        return;
    }
}