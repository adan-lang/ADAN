#include "utility/fsys.hh"

#include "tokens.hh"
#include  "lexer.hh"

Lexer::Lexer(
    LexerType lexer
) {
    this->source = lexer.source;
    this->pos = lexer.pos;
    this->line = lexer.line;
    this->col = lexer.col;
    this->abort = lexer.abort;
}

Lexer::~Lexer() { }