#include <iostream>
#include <fstream>

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