#pragma once

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <expected>
#include <memory>
#include <span>
#include <string>
#include <string_view>

#include "ast.hh"

class ASTLowerer {
    llvm::LLVMContext &context;
    llvm::IRBuilder<> builder;

    std::unique_ptr<llvm::Module> _module;

  public:
    ASTLowerer(llvm::LLVMContext &context, std::string_view module_name);

    /**
     *
     * Creates `i64 name()` and lowers `body` into it. Returns LLVM's verifier.
     *
     * If the result isn't valid IR, send a message.
     *
     * Temporary until the AST has a function node.
     */
    std::expected<llvm::Function *, std::string> lower_function(std::string_view name, std::span<const Stmt> body);

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
};