#include <print>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "lexer.hh"
#include "tools.hh"

static const std::string COMPILER_VERSION = "version-1.0.3";

static inline void show_main_message()
{
    std::println(
        "\n"
        "\033[1mUsage:\033[0m adan \033[2m[options]\033[0m <file>\n"
        "\n"
        "\033[1mArguments:\033[0m\n"
        "    \033[32m--version\033[0m, \033[32m-v\033[0m     Get your version of ADAN that's on your machine.\n"
        "    \033[32m--help\033[0m, \033[32m-h\033[0m        Display this message.\n"
        "\n"
        "ADAN is in BETA and is a passion project of mine.\n"
        "\n"
        "Support the development of ADAN by starring the\n"
        "repository over on GitHub!\n"
        "\n"
        "  \u27a4  \033[1;3;34mhttps://github.adan.sh/\033[0m\n"
        "\n"
        "For reporting a bug, please create a new issue on GitHub."
        "\n");
}

int main(int argc, char *argv[])
{
    bool show_help = false;
    bool show_version = false;
    bool test_lexer = false;
    bool test_parser = false;
    bool test_ast = false;

    std::string file_path;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h")
            show_help = true;
        else if (arg == "--version" || arg == "-v")
            show_version = true;
        else if (arg == "--lex-test" || arg == "-lt")
            test_lexer = true;
        else if (arg == "--parse-test" || arg == "-pt")
            test_parser = true;
        else if (arg == "--ast-test" || arg == "-at")
            test_ast = true;
        else
            file_path = arg;
    }

    const std::string source{read_file(file_path)};
    Lexer lexer(source);

    auto next{lexer.next()};

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

    while (next.kind != TokenKind::Eof)
    {
        if (test_lexer)
            std::println("{}", next.to_string());
        next = lexer.next();
    }

    return 0;
}