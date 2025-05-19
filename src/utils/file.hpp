#ifndef FILE_HPP_
#define FILE_HPP_

#include <filesystem>

namespace fs = std::filesystem;

using Path = fs::path;

CONST char* SRC_EXTENSTION = ".wo";
CONST char* OUT_EXTENSION  = ".out";

inline bool real_loc(const fs::path& fp) {
    return fs::exists(fp);
}

inline bool is_directory(const fs::path& fp) {
    return fs::is_directory(fp);
}

inline bool matches_ext(const fs::path& fp, const char* ext) {
    return fp.extension() == ext;
}

#endif // FILE_HPP_