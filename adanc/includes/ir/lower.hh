#pragma once

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "ast-visitor.hh"
#include "ast.hh"
#include "diagnostics.hh"
#include "types.hh"

class ASTLowerer : public ASTVisitor {
    llvm::LLVMContext &context;
    llvm::IRBuilder<> builder;

    std::unique_ptr<llvm::Module> _module;
    Diagnostics &diagnostics;
    std::unordered_map<std::string, llvm::Value *> local_values;

    llvm::Value *result{};

  public:
    ASTLowerer(llvm::LLVMContext &context, std::string_view module_name, Diagnostics &diagnostics);

    void declare_function(std::string_view name, const TypePtr &function_type);
    std::expected<llvm::Function *, std::string> lower_function(const FuncDecl &declaration);

    const llvm::Module &module() const
    {
        return *_module;
    }

    /**
     *
     * @brief Move the finished LLVM module out of the lowerer.
     */
    std::unique_ptr<llvm::Module> take()
    {
        return std::move(_module);
    }

  private:
    llvm::Type *lower_type(const TypePtr &type, bool allow_void = false);
    llvm::Value *lower_expr(Expr &expr);

    void visit(IntLiteral &node) override;
    void visit(FloatLiteral &node) override;
    void visit(BoolLiteral &node) override;
    void visit(StringLiteral &node) override;
    // void visit(InterpolatedString &node) override;
    // void visit(ArrayLiteral &node) override;

    // void visit(ArrayIndex &node) override;
    void visit(CallExpr &node) override;
    void visit(Identifier &node) override;
    // void visit(BinaryExpr &node) override;
    // void visit(Identifier &node) override;
    // void visit(UnaryExpr &node) override;

    void lower_stmt(Stmt &stmt);

    void visit(ReturnStmt &node) override;
    void visit(ExprStmt &node) override;
    void visit(IfStmt &node) override;
    // void visit(NumericForStmt &node) override;
    // void visit(ForInStmt &node) override;
    // void visit(WhileStmt &node) override;

    // void visit(LocalDecl &node) override;
    // void visit(FuncDecl &node) override;
};