#ifndef IO_HPP_
#define IO_HPP_

#include <iostream>
#include <expected>
#include <variant>
#include <filesystem>

#include "diagnostic.hpp"

namespace fs = std::filesystem;


struct IoFile {
    fs::path file;
    fs::perms file_perms;

    IoFile() = default;
    IoFile(std::string path) : file(path), file_perms(fs::perms::none) {};

    inline std::string as_str() { return file.native(); }
    
    auto can_read() -> bool;
    auto can_write() -> bool;

    auto validate(
        const std::string& extension, 
        bool for_read, 
        bool for_write
    ) -> std::expected<std::monostate, Diagnostic>;
};

//! Aliasing types for cleaner code.
using InputFile = IoFile;
using OutputFile = IoFile;

#endif // IO_HPP_