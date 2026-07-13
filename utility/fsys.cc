#include <iostream>
#include <fstream>
#include <string_view>
#include <string>

#include "fsys.hh"

std::string read_file(
    std::string file_path
) {
    std::ifstream file(file_path);
    std::string line;
    std::string file_contents;

    while (std::getline(file, line))
    {
        file_contents += line + "\n";
    }

    return file_contents;
}

std::string get_line(
    std::string_view source,
    int line_num
) {
    int current_line = 1;
    size_t start{};

    for (size_t i{}; i < source.size(); ++i)
    {
        if (source[i] == '\n')
        {
            if (current_line == line_num)
            {
                return std::string(source.substr(start, i - start));
            }

            current_line++;
            start = i + 1;
        }
    }

    if (current_line == line_num)
    {
        return std::string(source.substr(start));
    }

    return "";
}