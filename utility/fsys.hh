#pragma once

#include <string>
#include <string_view>

std::string read_file(
    std::string file_path
);

std::string get_line(
    std::string_view source,
    int line_num
);