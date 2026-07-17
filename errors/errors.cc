#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "errors.hh"

void report(
    Error& error
) {
    const char* level_color = (error.severity == SEVERITY_ERROR) ? "\x1b[1;31m" : "\x1b[1;33m";
    const char* level_word = (error.severity == SEVERITY_ERROR) ? "error" : "warning";

    std::string line_num = std::to_string(error.line_num);
    std::string pad(line_num.size(), ' ');

    std::cout <<

        level_color << level_word << "[" << error.code << "]" << "\x1b[0m" << "\x1b[1m" << ": " << error.message << "\x1b[0m"
        
    << "\n";

    std::cout <<
    
        pad << "\x1b[1m" << " --> " << "\x1b[0m" << error.line_num << ":" << error.col_start
    
    << "\n";

    std::cout <<

        pad << "\x1b[1m" << " |" << "\x1b[0m"

    << "\n";

    std::cout <<

        "\x1b[1m" << "\x1b[1m" << line_num << " | " << "\x1b[0m" << error.line

    << "\n";

    std::cout << pad << " " << "\x1b[1m" << "|" << "\x1b[0m" << " ";

    for (int i{1}; i < error.col_start; ++i)
    {
        std::cout << ' ';
    }

    std::cout << level_color;

    int span = std::max(1, error.col_end - error.col_start);

    for (int i{}; i < span; ++i)
    {
        std::cout << '^';
    }

    std::cout <<
    
        "\x1b[0m"
        
    << "\n";

    if (error.hint.has_value()) // exists
    {
        std::cout <<

            pad << " " << "\x1b[1m" << "|" << "\x1b[0m"

        << "\n\n";

        std::cout <<

            "\x1b[1;32m" << "Help" << "\x1b[0m" << ": " << *error.hint

        << "\n";
    }

    std::cout << "\n";
}