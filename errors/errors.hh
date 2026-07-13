#pragma once

#include <string>
#include <optional>

enum ErrorSeverity
{
    SEVERITY_ERROR,
    SEVERITY_WARN
};

struct Error
{
    std::string code;                    // error code (like "E0003")
    std::string message;                 // a short title
    std::string line;                    // the entire line causing the issue
    ErrorSeverity severity;              // how bad the error is
    int line_num;
    int col_start;                       // 1-indexed, inclusive
    int col_end;                         // 1-indexed, exclusive
    std::optional<std::string> hint;     // help suggestion, if any
};

void report(
    Error& error
);
