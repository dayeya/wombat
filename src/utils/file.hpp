#ifndef FILE_HPP_
#define FILE_HPP_

#include <iostream>
#include <filesystem>
#include "common.hpp"

namespace fs = std::filesystem;

CONST char* SRC_EXTENSTION = ".wo";
CONST char* OUT_EXTENSION  = ".obj";

bool real_loc(const fs::path& fp) {
    return fs::exists(fp);
}

bool is_directory(const fs::path& fp) {
    return fs::is_directory(fp);
}

bool matches_ext(const fs::path& fp, const char* ext) {
    return fp.extension() == ext;
}

#endif // FILE_HPP_
