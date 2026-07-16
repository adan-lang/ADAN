#include <string>

#include "type.hh"

bool Type::equals(
    const Type& other
) const {
    if (kind == TypeKind::ANY || other.kind == TypeKind::ANY)
    {
        return true;
    }

    if (kind != other.kind)
    {
        return false;
    }

    if (is_nullable != other.is_nullable)
    {
        return false;
    }

    switch (kind)
    {
        case TypeKind::ARRAY:
        {
            if (element_type == nullptr || other.element_type == nullptr)
            {
                return element_type == other.element_type;
            }

            return element_type->equals(*other.element_type);
        }

        case TypeKind::FUNCTION:
        {
            if (return_type == nullptr || other.return_type == nullptr)
            {
                if (return_type != other.return_type)
                {
                    return false;
                }
            }

            else if (!return_type->equals(*other.return_type))
            {
                return false;
            }

            // all param lists *must* match in count and pairwise in type
            if (param_types.size() != other.param_types.size())
            {
                return false;
            }

            for (size_t i{}; i < param_types.size(); i++)
            {
                if (!param_types[i]->equals(*other.param_types[i]))
                {
                    return false;
                }
            }

            return true;
        }

        default:
            return true;
    }
}

std::string Type::make_string() const
{
    std::string result;

    switch (kind)
    {
        case TypeKind::STRING:
            result = "string";
            break;
        case TypeKind::CHAR:
            result = "char";
            break;
        case TypeKind::NUMBER:
            result = "number";
            break;
        case TypeKind::INT:
            result = "int";
            break;
        case TypeKind::FLOAT:
            result = "float";
            break;
        case TypeKind::BOOL:
            result = "bool";
            break;
        case TypeKind::NONE:
            result = "none";
            break;
        case TypeKind::VOID:
            result = "void";
            break;
        case TypeKind::AUTO:
            result = "auto";
            break;
        case TypeKind::ANY:
            result = "any";
            break;
        case TypeKind::ARRAY:
            result = (element_type != nullptr) ? element_type->make_string() + "[]" : "unknown[]";
            break;

        case TypeKind::FUNCTION:
        {
            result = "function(";
            
            for (size_t i{}; i < param_types.size(); i++)
            {
                result += param_types[i]->make_string();

                if (i + 1 < param_types.size())
                {
                    result += ", ";
                }
            }

            result += ")";
            result += " -> ";
            result += (return_type != nullptr) ? return_type->make_string() : "void";
            
            break;
        }
    }

    if (is_nullable)
    {
        result += "?";
    }

    return result;
}

Type* Type::make_function(
    std::vector<Type*> params,
    Type* explicit_type = nullptr
) {
    Type* func = new Type();

    func->kind = TypeKind::FUNCTION;
    func->param_types = std::move(params);
    func->return_type = (explicit_type != nullptr) ? explicit_type : new Type();

    return func;
}

Type::Type()
    : kind(TypeKind::AUTO),
    is_nullable(false),
    element_type(nullptr),
    return_type(nullptr)
{ }

Type::~Type()
{
    delete element_type;
    delete return_type;

    for (Type* parameter : param_types)
    {
        delete parameter;
    }
}