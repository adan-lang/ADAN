#pragma once

#include <string>

enum ErrorSeverity
{
    SEVERITY_ERROR,
    SEVERITY_WARN
};

struct Error
{
    std::string message;      // error message
    std::string line;         // the entire line causing the issue
    ErrorSeverity severity;   // how bad the error is
    int line_num;             // the line number of the line
    int col_num;              // the column, or character specifically causing the issue
};

void report(
    Error& error
);
