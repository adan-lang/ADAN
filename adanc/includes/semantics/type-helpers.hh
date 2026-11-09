#pragma once

#include <cstddef>
#include <format>
#include <string>

#include "types.hh"

/**
 *
 * @brief Returns whether or not `variable` appears in `type`.
 *
 * If the function returns false, that means `unify` can safely
 *  link the variable to the type.
 */
inline bool occurs(const TypePtr &variable, const TypePtr &type)
{
    const TypePtr resolved = prune(type);

    if (resolved == variable)
        return true;

    switch (resolved->kind)
    {
    case TypeKind::Int:
    case TypeKind::Float:
    case TypeKind::Bool:
    case TypeKind::String:
    case TypeKind::Void:
    case TypeKind::Var:
        return false;

    case TypeKind::Array:
        return occurs(variable, resolved->element);

    case TypeKind::Function:
        for (std::size_t i{}; i < resolved->params.size(); ++i)
            if (occurs(variable, resolved->params[i]))
                return true;

        return occurs(variable, resolved->result);
    }
}

inline bool type_can_match(const TypePtr &actual, const TypePtr &expected)
{
    const TypePtr resolved_actual = prune(actual);
    const TypePtr resolved_expected = prune(expected);

    if (resolved_actual->kind == TypeKind::Var || resolved_expected->kind == TypeKind::Var)
        return true;
    if (resolved_actual->kind != resolved_expected->kind)
        return false;

    switch (resolved_actual->kind)
    {
    case TypeKind::Array:
        return type_can_match(resolved_actual->element, resolved_expected->element);
    case TypeKind::Function:
        if (resolved_actual->params.size() != resolved_expected->params.size())
            return false;
        for (std::size_t i{}; i < resolved_actual->params.size(); ++i)
            if (!type_can_match(resolved_actual->params[i], resolved_expected->params[i]))
                return false;
        return type_can_match(resolved_actual->result, resolved_expected->result);
    case TypeKind::Int:
    case TypeKind::Float:
    case TypeKind::Bool:
    case TypeKind::String:
    case TypeKind::Void:
    case TypeKind::Var:
        return true;
    }

    return false;
}

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
        return std::format("\033[1;35mArray\033[0m\033[1;33m<\033[0m{}\033[1;33m>\033[0m",
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