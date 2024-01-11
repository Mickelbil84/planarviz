#include "Utils.h"

#include <fmt/core.h>
using fmt::print, fmt::format;

std::string getFileContent(std::string filename) {
    std::ifstream fp(filename.c_str());
    if (!fp.good()) throw std::runtime_error(format("Could not open file:\n\t{}\n", filename));

    std::stringstream buffer;
    buffer << fp.rdbuf();
    fp.close();

    return buffer.str();
}