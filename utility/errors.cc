#include <cstdlib>
#include   <print>

#include "errors.hh"

void report(
    Error& error
) {
    if (error.severity == SEVERITY_FATAL)
    {
        std::print("[FATAL Error]: {} on line {}:{}", error.message, error.line, error.col);
        std::exit(1);
    }

    else if (error.severity == SEVERITY_ERROR)
    {
        std::print("[Error]: {} on line {}:{}", error.message, error.line, error.col);
    }

    else if (error.severity == SEVERITY_WARN)
    {
        std::print("[Warning]: {} on line {}:{}", error.message, error.line, error.col);
    }
}