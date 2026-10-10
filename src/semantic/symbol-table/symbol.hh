#pragma once

#include <string>

#include "type.hh"

struct Symbol
{
    std::string name;
    TypePtr type;
    bool initialized;
};