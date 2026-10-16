#include <format>

#include "types.hh"

TypePtr make_int()
{
    make_type(TypeKind::Int);
}

TypePtr make_float()
{
    make_type(TypeKind::Float);
}

TypePtr make_bool()
{
    make_type(TypeKind::Bool);
}

TypePtr make_string()
{
    make_type(TypeKind::String);
}

TypePtr make_void()
{
    make_type(TypeKind::Void);
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

// ...
// made using AI
std::string type_to_string(const TypePtr &type)
{
    const TypePtr resolved = prune(type);

    switch (resolved->kind)
    {
    case TypeKind::Int:
        return "\033[1;35mInt\033[0m";
    case TypeKind::Float:
        return "\033[1;35mFloat\033[0m";
    case TypeKind::Bool:
        return "\033[1;35mBool\033[0m";
    case TypeKind::String:
        return "\033[1;35mString\033[0m";
    case TypeKind::Void:
        return "\033[1;35mVoid\033[0m";
    case TypeKind::Array:
        return std::format("\033[1;35mArray\033[0m \033[1;33mof\033[0m \033[1;33m\"\033[0m{}\033[1;33m\"\033[0m",
                           type_to_string(resolved->element));
    case TypeKind::Function:
    {
        std::string text = "\033[1;33m(\033[0m";
        for (std::size_t i = 0; i < resolved->params.size(); ++i)
        {
            if (i != 0)
                text += "\033[1;33m,\033[0m ";
            text += type_to_string(resolved->params[i]);
        }

        text += "\033[1;33m) ->\033[0m ";
        text += type_to_string(resolved->result);
        return text;
    }
    case TypeKind::Var:
        return "\033[1;36mt" + std::to_string(resolved->id) + "\033[0m";
    }

    return "\033[1;31mUnknown\033[0m";
}