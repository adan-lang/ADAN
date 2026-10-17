#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <print>

static inline std::string read_file(std::string source_file)
{
    std::ifstream file(source_file);
    if (!file.is_open())
    {
        std::println("Couldn't read file: {}", source_file);
        return "";
    }

    std::ostringstream string_stream;
    string_stream << file.rdbuf();

    return string_stream.str();
}