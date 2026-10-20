#include <format>

#include "types.hh"

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