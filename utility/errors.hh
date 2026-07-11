#pragma once

#include <string>

enum ErrorSeverity
{
    SEVERITY_FATAL,
    SEVERITY_ERROR,
    SEVERITY_WARN
};

struct Error
{
    std::string message;
    ErrorSeverity severity;
    int line;
    int col;
};

void report(
    Error& error
);
