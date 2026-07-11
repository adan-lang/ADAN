#include <vector>
#include <string>
#include <string_view>

#include "lexer.hh"
#include "suite.hh"

std::string_view token_name(
    TokenType type
);

std::string serialize_tokens(
    const std::vector<Token>& tokens
);

void run_lexer_tests();