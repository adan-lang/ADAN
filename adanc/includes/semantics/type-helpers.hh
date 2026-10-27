#pragma once

#include <cstddef>
#include <format>
#include <string>

#include "types.hh"

// inline TypePtr string_to_type(std::string type)
// {
//     if (type == "Int")
//         return make_int();
//     if (type == "Float")
//         return make_float();
//     if (type == "Bool")
//         return make_bool();
//     if (type == "String")
//         return make_string();
//     if (type == "Void")
//         return make_void();

//     return make_var();
// }

inline std::string type_to_string(const TypePtr &type)
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
        for (std::size_t i{}; i < resolved->params.size(); ++i)
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