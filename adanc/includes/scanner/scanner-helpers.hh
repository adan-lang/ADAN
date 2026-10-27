#pragma once

#include <cstdint>
#include <string_view>

enum class TokenKind : std::uint8_t;

std::string_view token_kind_name(TokenKind kind);