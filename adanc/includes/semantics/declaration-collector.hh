#pragma once

#include "ast-visitor.hh"
#include "scope-manager.hh"

class DeclarationCollector : public ASTVisitor {
  ScopeManager &symbol_table;

public:
  explicit DeclarationCollector(ScopeManager &symbols)
      : symbol_table(symbols) {}

  void visit(LocalDecl &stmt) override;
  void visit(FuncDecl &stmt) override;
  void visit(IfStmt &stmt) override;
  void visit(NumericForStmt &stmt) override;
  void visit(ForInStmt &stmt) override;
  void visit(WhileStmt &stmt) override;
};