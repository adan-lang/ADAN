#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <string>

struct Type;
using TypePtr = std::shared_ptr<Type>;

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

    // Some things need extra information, like arrays, functions, and variables.
    TypePtr element;
    std::vector<TypePtr> params;

    int id = 0;

    TypePtr link;
    TypePtr result;

    explicit Type(TypeKind type_kind) : kind{type_kind} {}
};

TypePtr make_int();
TypePtr make_float();
TypePtr make_bool();
TypePtr make_string();
TypePtr make_array(TypePtr element);
TypePtr make_function(std::vector<TypePtr> params, TypePtr result);
TypePtr make_var();

TypePtr prune(const TypePtr &type);
std::string type_to_string(const TypePtr &type);