#ifndef IO_HPP_
#define IO_HPP_

#include <iostream>
#include <expected>
#include <variant>
#include <filesystem>

#include "diagnostic.hpp"

namespace fs = std::filesystem;

//! Expecting an Error 'maybe :)'
//! When validating files we do not return a value on success. Rather, we just return a diagnostic if needed.
using ExpectedIoDiagnostic = std::expected<std::monostate, Diagnostic>;

struct IoFile {
    fs::path file;
    fs::perms file_perms;

    IoFile() = default;
    IoFile(std::string path) : file(path), file_perms(fs::perms::none) {};

    auto can_read() -> bool;
    auto can_write() -> bool;
    auto validate_input_file() -> ExpectedIoDiagnostic;
    auto validate_out_file() -> ExpectedIoDiagnostic;
};

//! Aliasing types for cleaner code.
using InputFile = IoFile;
using OutputFile = IoFile;

#endif // IO_HPP_