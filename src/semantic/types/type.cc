#include "type.hh"

TypePtr make_int()
{
    return std::make_shared<Type>(TypeKind::Int);
}

TypePtr make_float()
{
    return std::make_shared<Type>(TypeKind::Float);
}

TypePtr make_bool()
{
    return std::make_shared<Type>(TypeKind::Bool);
}

TypePtr make_string()
{
    return std::make_shared<Type>(TypeKind::String);
}

TypePtr make_array(TypePtr element)
{
}

TypePtr make_function(std::vector<TypePtr> params, TypePtr result)
{
}

TypePtr make_var()
{
}

TypePtr prune(const TypePtr &type)
{
}