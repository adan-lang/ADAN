#include <string>

#include "lexer.hh"

Lexer::Lexer(std::string source_code)
    : source_code(source_code), pos(0) {}

Lexer::~Lexer() {}