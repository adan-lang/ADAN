#include <array>
#include <string_view>

enum class ErrorCode
{
    UnterminatedMultilineComment,
    IncompleteFloat,
    UnterminatedString,
    InvalidCharacter,
    UnexpectedEOF,

    // don't move this, leave this at the bottom
    COUNT
};

constexpr std::array<std::string_view, static_cast<size_t>(ErrorCode::COUNT)> ErrorMessages = {
    "Unterminated multiline comment",
    "Incomplete floating point value",
    "Unterminated string",
    "Invalid character",
    "Unexpectedly reached the end of the file",
};

constexpr std::string_view message(
    ErrorCode code
) {
    return ErrorMessages[static_cast<size_t>(code)];
}