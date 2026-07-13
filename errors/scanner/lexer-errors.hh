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

constexpr std::array<ErrorInfo, static_cast<size_t>(ErrorCode::COUNT)> ErrorTable = {{
    {"E0001", "unterminated multiline comment",     "close the comment with the same number of '#' you opened it with"},
    {"E0002", "incomplete floating point value",    "add a digit after the decimal point, or remove it"},
    {"E0003", "unterminated string literal",        "add a closing '\"'"},
    {"E0004", "invalid character",                  ""},
    {"E0005", "unexpected end of file",             ""},
}};

constexpr std::string_view message(
    ErrorCode code
) {
    return ErrorMessages[static_cast<size_t>(code)];
}