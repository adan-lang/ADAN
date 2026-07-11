#include <cstdlib>
#include <iostream>

#include "errors.hh"

void report(
    Error& error
) {
    if (error.severity == SEVERITY_FATAL)
    {
        std::cout << "[FATAL Error]: " << error.message << " on line " << error.line << ":" << error.col << "\n";
    }

    else if (error.severity == SEVERITY_ERROR)
    {
        std::cout << "[Error]: " << error.message << " on line " << error.line << ":" << error.col << "\n";
    }

    else if (error.severity == SEVERITY_WARN)
    {
        std::cout << "[Warning]: " << error.message << " on line " << error.line << ":" << error.col << "\n";
    }
}