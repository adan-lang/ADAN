#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include <cstddef>
#include <cstdio>
#include <print>
#include <string>
#include <utility>

#include "built-ins.hh"
#include "declaration-collector.hh"
#include "diagnostics.hh"
#include "lower.hh"
#include "parser.hh"
#include "scope-manager.hh"
#include "tools.hh"
#include "type-checker.hh"

static constexpr std::string COMPILER_VERSION = "version-1.2.8";

static inline void show_main_message()
{
    std::println("\n"
                 "\033[1mUsage:\033[0m adan \033[2m[options]\033[0m <file>\n"
                 "\n"
                 "\033[1mArguments:\033[0m\n"
                 "    \033[32m--version\033[0m, \033[32m-v\033[0m     Get your "
                 "version of ADAN that's on your machine.\n"
                 "    \033[32m--help\033[0m, \033[32m-h\033[0m        Display "
                 "this message.\n"
                 "    \033[32m--emit-ir\033[0m             Emit LLVM IR for supported functions.\n"
                 "\n"
                 "ADAN is in BETA and is a passion project of mine.\n"
                 "\n"
                 "Support the development of ADAN by starring the\n"
                 "repository over on GitHub!\n"
                 "\n"
                 "  \u27a4  \033[1;3;34mhttps://git.adan.sh/\033[0m\n"
                 "\n"
                 "For reporting a bug, please create a new issue on GitHub."
                 "\n");
}

int main(const int argc, char *argv[])
{
    bool show_help = false;
    bool show_version = false;

    std::string file_path;

    for (int i = 1; i < argc; i++)
    {
        if (std::string arg = argv[i]; arg == "--help" || arg == "-h")
            show_help = true;
        else if (arg == "--version" || arg == "-v")
            show_version = true;
        else
            file_path = arg;
    }

    if (show_help)
    {
        show_main_message();
        return 0;
    }

    if (show_version)
    {
        std::println("Compiler on {}\n", COMPILER_VERSION);
        return 0;
    }

    if (file_path.empty())
    {
        show_main_message();
        return 1;
    }

    const std::string source{read_file(file_path)};

    Diagnostics diagnostics;

    Parser parser(source, diagnostics);
    auto statements = parser.parse();

    if (diagnostics.has_errors())
    {
        diagnostics.output();
        return 1;
    }

    Block program;
    program.stmts = std::move(statements);

    ScopeManager symbol_table{diagnostics};
    symbol_table.enter_scope();

    declare_builtins(symbol_table);

    try
    {
        DeclarationCollector collector{symbol_table};
        for (std::size_t i{}; i < program.stmts.size(); ++i)
        {
            program.stmts[i]->accept(collector);
        }
    }
    catch (const DiagnosticAbort &)
    {}

    symbol_table.set_shadow_warnings_enabled(false);

    if (diagnostics.has_errors())
    {
        diagnostics.output();
        return 1;
    }

    TypeChecker type_checker{symbol_table, diagnostics};
    try
    {
        type_checker.check(program);
        type_checker.finalize();
    }
    catch (const DiagnosticAbort &)
    {}

    if (diagnostics.has_errors())
    {
        diagnostics.output();
        return 1;
    }

    llvm::LLVMContext llvm_context;
    ASTLowerer lowerer{llvm_context, file_path, diagnostics};

    try
    {
        for (const auto &stmt : program.stmts)
        {
            const auto *function = dynamic_cast<const FuncDecl *>(stmt.get());
            const TypePtr function_type = prune(symbol_table.lookup(function->name)->type);

            lowerer.declare_function(function->name, function_type);
        }

        for (const auto &stmt : program.stmts)
        {
            const auto *function = dynamic_cast<const FuncDecl *>(stmt.get());

            auto lowered = lowerer.lower_function(*function);
            if (!lowered)
                diagnostics.error("Failed to lower function '" + function->name + "': " + lowered.error());
        }
    }
    catch (const DiagnosticAbort &)
    {}

    diagnostics.output();
    if (diagnostics.has_errors())
        return 1;

    lowerer.module().print(llvm::outs(), nullptr);

    return 0;
}