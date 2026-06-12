#include "utility/fsys.hh"

#include "tokens.hh"
#include  "lexer.hh"

Lexer::Lexer(
    std::string source,
    int pos,
    int line,
    int col,
    bool abort
) {
    this->source = source;
    this->pos = pos;
    this->line = line;
    this->col = col;
    this->abort = abort;
}

Lexer::~Lexer() { }