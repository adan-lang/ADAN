#pragma once

#include <string>

enum ErrorSeverity
{
    SEVERITY_ERROR,
    SEVERITY_WARN
};

struct Error
{
    std::string message;      // Error message
    std::string line;         // The entire line causing the issue
    ErrorSeverity severity;   // How bad the error is
    int line;                 // The line number of the line
    int col;                  // The column, or character specifically causing the issue
};

void report(
    Error& error
);
