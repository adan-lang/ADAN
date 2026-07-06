#include <iostream>
#include <string>
#include <string_view>

std::string __LOCAL_VERSION__ = "1.0.c"; // probably not an ideal way of doing this

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
        std::cout << "Usage: " << argv[0] << " <arg>\n";
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
                    std::cout << __LOCAL_VERSION__ << "\n";
                }
                
                break;
            }
        }
    }
    
    return 0;
}