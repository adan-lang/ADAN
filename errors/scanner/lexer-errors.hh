#pragma once

#include <array>
#include <string_view>

enum class ErrorCode
{
    UnterminatedMultilineComment,
    IncompleteFloat,
    UnterminatedString,
    InvalidCharacter,
    UnexpectedEOF,

    COUNT
};

struct ErrorInfo
{
    std::string_view code;
    std::string_view message;
    std::string_view hint;
};

constexpr std::array<ErrorInfo, static_cast<size_t>(ErrorCode::COUNT)> ErrorMessages = {{
    {"E0001", "Unterminated multiline comment",     "Close the comment with the same number of '#' you opened it with"},
    {"E0002", "Incomplete floating point value",    "Add a digit after the decimal point, or remove it"},
    {"E0003", "Unterminated string literal",        "Add a closing '\"'"},
    {"E0004", "Invalid character",                  ""},
    {"E0005", "Unexpected end of file",             ""},
}};

constexpr const ErrorInfo& info(
    ErrorCode code
) {
    return ErrorMessages[static_cast<size_t>(code)];
}