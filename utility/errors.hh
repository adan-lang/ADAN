#pragma

#include <string>

typedef enum ErrorSeverity
{
    SEVERITY_FATAL,
    SEVERITY_ERROR,
    SEVERITY_WARN
};

typedef struct Error
{
    std::string message;
    ErrorSeverity severity;
    int line;
    int col;
};

void report(
    Error& error
);