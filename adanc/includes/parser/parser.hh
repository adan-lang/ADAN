#pragma once

#include <format>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "ast.hh"
#include "diagnostics.hh"
#include "scanner-helpers.hh"
#include "scanner.hh"

class Parser {
  Lexer lexer;
  Token current_token;

  Diagnostics &diagnostics;

  [[noreturn]] void report_error(std::string message) {
    diagnostics.error(std::move(message), current_token.line,
                      current_token.column);
  }

public:
  explicit Parser(std::string_view input, Diagnostics &diagnostics)
      : lexer{input, diagnostics}, diagnostics{diagnostics} {}

  std::vector<std::unique_ptr<Stmt>> parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    try {
      current_token = lexer.next();
      while (current_token.kind != TokenKind::Eof)
        statements.push_back(parse_stmt());
    } catch (const DiagnosticAbort &) {
    }

    return statements;
  }

private:
  Token advance() { return std::exchange(current_token, lexer.next()); }

  Token expect(const TokenKind kind) {
    if (current_token.kind != kind) {
      report_error(std::format(
          "Expected {}, got {} (\"{}\")", token_kind_name(kind),
          token_kind_name(current_token.kind), current_token.lexeme));
    }

    return advance();
  }

  template <typename... T> bool matches(T... types) const {
    return ((current_token.kind == types) || ...);
  }

  Block parse_block();

  std::unique_ptr<Expr> parse_expr();
  std::unique_ptr<Expr> parse_comparison();
  std::unique_ptr<Expr> parse_additive();
  std::unique_ptr<Expr> parse_multiplicative();
  std::unique_ptr<Expr> parse_unary();
  std::unique_ptr<Expr> parse_power();
  std::unique_ptr<Expr> parse_postfix();
  std::unique_ptr<Expr> parse_primary();
  std::unique_ptr<Expr> parse_array_literal();
  std::unique_ptr<Expr> parse_template_string();

  std::unique_ptr<Stmt> parse_stmt();
  std::unique_ptr<Stmt> parse_return();
  std::unique_ptr<Stmt> parse_local();
  std::unique_ptr<Stmt> parse_function();
  std::unique_ptr<Stmt> parse_if();
  std::unique_ptr<Stmt> parse_for();
  std::unique_ptr<Stmt> parse_while();
};
