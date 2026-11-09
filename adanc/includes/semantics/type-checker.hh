#pragma once

#include "ast-visitor.hh"
#include "ast.hh"
#include "operator-table.hh"
#include "scope-manager.hh"
#include "types.hh"

#include <vector>

class TypeChecker : public ASTVisitor
{
    struct PendingBinaryOperator
    {
        TypePtr lhs;
        BinaryOp op;
        TypePtr rhs;

        TypePtr result;
        
        bool target_widenable{};
    };

    struct PendingUnaryOperator
    {
        TypePtr operand;
        UnaryOp op;

        TypePtr result;
    };

    ScopeManager &symbol_table;
    OperatorTable operator_table;

    TypePtr last_inferred_type;
    TypePtr curr_func_result_type;

    std::vector<PendingBinaryOperator> pending_binary_operators;
    std::vector<PendingUnaryOperator> pending_unary_operators;

    void resolve_pending_operators();

public:
    explicit TypeChecker(ScopeManager &symbols) : symbol_table(symbols) {}

    TypePtr infer(Expr &expr);

    void check(Stmt &stmt);
    void check(Block &block);
    
    void finalize();

    // Visitors

    void visit(IntLiteral &node) override;
    void visit(FloatLiteral &node) override;
    void visit(BoolLiteral &node) override;
    void visit(StringLiteral &node) override;
    void visit(InterpolatedString &node) override;
    void visit(ArrayLiteral &node) override;

    void visit(ArrayIndex &node) override;
    void visit(CallExpr &node) override;
    void visit(BinaryExpr &node) override;
    void visit(Identifier &node) override;
    void visit(UnaryExpr &node) override;

    void visit(ReturnStmt &node) override;
    void visit(ExprStmt &node) override;
    void visit(IfStmt &node) override;
    void visit(NumericForStmt &node) override;
    void visit(ForInStmt &node) override;
    void visit(WhileStmt &node) override;

    void visit(LocalDecl &node) override;
    void visit(FuncDecl &node) override;
};