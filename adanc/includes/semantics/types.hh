#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "diagnostics.hh"

struct Type;
using TypePtr = std::shared_ptr<Type>;

enum class TypeKind : uint8_t {
    Int,
    Float,
    Bool,
    String,
    Array,
    Function,
    Void,
    Var,
};

struct Type {
    TypeKind kind;

    TypePtr element;
    std::vector<TypePtr> params;

    int id{};

    TypePtr link;
    TypePtr result;

    explicit Type(TypeKind type_kind) : kind{type_kind} {}

    bool operator==(const Type &other) const
    {
        const Type *lhs = this;
        const Type *rhs = &other;

        while (lhs->kind == TypeKind::Var && lhs->link)
            lhs = lhs->link.get();
        while (rhs->kind == TypeKind::Var && rhs->link)
            rhs = rhs->link.get();

        if (lhs->kind != rhs->kind)
            return false;

        const auto equal_ptr = [](const TypePtr &left, const TypePtr &right) { return left && right ? *left == *right : !left && !right; };

        switch (lhs->kind)
        {
        case TypeKind::Array:
            return equal_ptr(lhs->element, rhs->element);

        case TypeKind::Function:
            if (lhs->params.size() != rhs->params.size())
                return false;

            for (std::size_t i{}; i < lhs->params.size(); ++i)
            {
                if (!equal_ptr(lhs->params[i], rhs->params[i]))
                    return false;
            }

            return equal_ptr(lhs->result, rhs->result);

        case TypeKind::Var:
            return lhs->id == rhs->id;

        case TypeKind::Int:
        case TypeKind::Float:
        case TypeKind::Bool:
        case TypeKind::String:
        case TypeKind::Void:
            return true;
        }

        return false;
    }
};

template <typename T> static inline TypePtr make_type(T type_kind)
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

void unify(TypePtr a, TypePtr b, Diagnostics &diagnostics);