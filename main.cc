#include <iostream>
#include <string>
#include <string_view>

std::string __LOCAL_VERSION__ = "1.0.cb"; // probably not an ideal way of doing this

struct Option {
    std::string_view short_name;
    std::string_view long_name;
};

constexpr Option options[] = {
    {"v", "version"},
    {"h", "help"},
};

int main(
    int argc,
    char* argv[]
) {
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [options] <file>\n";
        return 1;
    }

    for (int i{1}; i < argc; i++)
    {
        std::string_view arg = argv[i];

        bool is_long = arg.starts_with("--");

        if (is_long)
        {
            arg.remove_prefix(2);
        }

        else if (arg.starts_with("-"))
        {
            arg.remove_prefix(1);
        }

        for (const auto& option : options)
        {
            if ((is_long && arg == option.long_name) || (!is_long && arg == option.short_name))
            {
                if (arg == "v" || arg == "version")
                {
                    std::cout << "ADAN version " << __LOCAL_VERSION__ << "\n";
                }

                else if (arg == "h" || arg == "help")
                {
                    std::cout <<

                        "\x1b[1mUsage\x1b[0m: " << argv[0] << " \x1b[2m[options]\x1b[0m <file>" << "\n\n" <<
                        "\x1b[1mArguments\x1b[0m:" << "\n" <<
                        "   \x1b[32m--version\x1b[0m, \x1b[32m-v\x1b[0m       Get your version of ADAN that's on your machine." << "\n" <<
                        "   \x1b[32m--help\x1b[0m, \x1b[32m-h\x1b[0m          Display this message." << "\n\n" <<
                        
                        "Support the development of ADAN by starring the " << "\n" <<
                        "repository on GitHub." << "\n\n" <<
                        
                        "  → \x1b[3m\x1b[34m\x1b[1mhttps://github.adan.sh/\x1b[0m" << "\n\n" <<

                        "For reporting a bug, create a new issue on GitHub."
                        
                    << "\n";
                }
                
                break;
            }
        }
    }
    
    return 0;
}