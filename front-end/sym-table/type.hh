#pragma once

#include <vector>
#include <string>

enum class TypeKind
{
    STRING,
    CHAR,
    NUMBER,
    INT,
    FLOAT,
    BOOL,
    NONE,
    VOID,
    AUTO,
    FUNCTION,
    ARRAY
};

class Type
{
public:
    TypeKind kind;
    bool is_nullable;
    Type* element_type; // only meaningful if --> `kind == ARRAY`
    std::vector<Type*> param_types; // only meaningful if --> `kind == FUNCTION`
    Type* return_type;              // same for this :P

    bool equals(
        const Type& other
    ) const;

    std::string make_string() const;

    Type();

    ~Type();
};