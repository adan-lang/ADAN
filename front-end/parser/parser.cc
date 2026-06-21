#include "parser.hh"

Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)), pos(0)
{

}

Parser::~Parser() { }