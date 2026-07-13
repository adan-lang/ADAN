#include <cstdlib>
#include <iostream>

#include "errors.hh"

void report(
    Error& error
) {
    // if (error.severity == SEVERITY_ERROR)
    // {
    //     std::cout << "\x1b[1;31m[Error]\x1b[0m: " << error.message << " on line " << error.line_num << ":" << error.col_num << "\n\n";
    // }

    // else if (error.severity == SEVERITY_WARN)
    // {
    //     std::cout << "\x1b[1;33m[Warning]\x1b[0m: " << error.message << " on line " << error.line_num << ":" << error.col_num << "\n\n";
    // }

    const char* level_color = (error.severity == SEVERITY_ERROR) ? "\x1b[1;31m" : "\x1b[1;33m";
    const char* level_word = (error.severity == SEVERITY_ERROR) ? "error" : "warning";

    std::string line_num = std::to_string(error.line_num);
    std::string pad(line_num.size(), ' ');

    // @todo header, location, gutter + source, caret span, hint
}