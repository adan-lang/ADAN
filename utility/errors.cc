#include <cstdlib>
#include <iostream>

#include "errors.hh"

void report(
    Error& error
) {
    if (error.severity == SEVERITY_ERROR)
    {
        std::cout << "\x1b[1;31m[Error]\x1b[0m: " << error.message << " on line " << error.line << ":" << error.col << "\n\n";
    }

    else if (error.severity == SEVERITY_WARN)
    {
        std::cout << "\x1b[1;33m[Warning]\x1b[0m: " << error.message << " on line " << error.line << ":" << error.col << "\n\n";
    }
}