#pragma once

#include <memory>
#include <vector>
#include <cstdint>

enum class TypeKind : uint8_t
{
    Int,
    Float,
    Bool,
    String,
    Array,
    Function,
    Var,
};

struct Type
{
    TypeKind kind;

    // Some things need extra information, like arrays, functions,
    //  and variables.
    std::shared_ptr<Type> element;
    std::vector<std::shared_ptr<Type>> params;

    int id = 0;
    std::shared_ptr<Type> link;

    explicit Type(TypeKind type_kind) : kind{type_kind} {}
};