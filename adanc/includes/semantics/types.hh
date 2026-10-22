#pragma once

#include <vector>

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
    Void,
    Var,
};

struct Type
{
    TypeKind kind;

    TypePtr element;
    std::vector<TypePtr> params;

    int id{};

    TypePtr link;
    TypePtr result;

    explicit Type(TypeKind type_kind) : kind{type_kind} {}
};

template <typename T>
static inline TypePtr make_type(T type_kind)
{
    return std::make_shared<Type>(type_kind);
}

TypePtr make_int();
TypePtr make_float();
TypePtr make_bool();
TypePtr make_string();
TypePtr make_void();
TypePtr make_array(TypePtr element);
TypePtr make_function(std::vector<TypePtr> params, TypePtr result);
TypePtr make_var();

TypePtr prune(const TypePtr &type);