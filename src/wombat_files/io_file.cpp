#include "io_file.hpp"
#include "diagnostic.hpp"

#include <expected>

auto IoFile::can_read() -> bool {
    return (file_perms & fs::perms::owner_read) != fs::perms::none;
}

auto IoFile::can_write() -> bool {
    return (file_perms & fs::perms::owner_write) != fs::perms::none;
}

auto IoFile::validate_input_file() -> ExpectedIoDiagnostic {
    if(!fs::exists(file)) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { file.generic_string() + " does not exist" },
            { 
                Suggestion("provide wombat with a file that matches the correct criteria"), 
                Suggestion("see wombat-docs for more information")
            },
            std::nullopt
        );
    }

    if(fs::is_directory(file)) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { file.generic_string() + " is a directory" },
            { Suggestion("provide an existing, valid .wo source file") },
            std::nullopt
        );
    }

    if(file.extension() != ".wo") {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { file.extension().generic_string() + " is not the expected extension. use [FILE].wo instead" },
            { Suggestion("see wombat-docs for more information") },
            std::nullopt
        );
    }

    //! file_ was empty.
    //! Can occur if component-validation on file_ failed.  
    if(file.empty()) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { "cannot operate on an empty path" },
            { 
                Suggestion("provide wombat with a file that matches the correct criteria"), 
                Suggestion("see wombat-docs for more information") 
            },
            std::nullopt
        );
    }

    // Given file is a valid wombat-source-file
    file_perms = fs::status(file).permissions();

    //! not-allowed to read from file.
    if(!can_read()) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { "file permissions denied, canonot read from " + file.filename().generic_string() },
            { Suggestion("change permissions to allow reading") },
            std::nullopt
        );
    }

    // All validation were successful.
    return std::monostate{};
}

auto IoFile::validate_out_file() -> ExpectedIoDiagnostic {
    if(!fs::exists(file)) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { file.generic_string() + " does not exist" },
            { 
                Suggestion("provide wombat with a file that matches the correct criteria"), 
                Suggestion("see wombat-docs for more information")
            },
            std::nullopt
        );
    }

    if(fs::is_directory(file)) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { file.generic_string() + " is a directory" },
            { Suggestion("provide an existing, valid .wo source file") },
            std::nullopt
        );
    }

    if(file.extension() != ".o") {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { file.extension().generic_string() + " is not the expected extension. use [FILE].o instead" },
            { Suggestion("see wombat-docs for more information") },
            std::nullopt
        );
    }

    //! file_ was empty.
    //! Can occur if component-validation on file_ failed.  
    if(file.empty()) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { "cannot operate on an empty path" },
            { 
                Suggestion("provide wombat with a file that matches the correct criteria"), 
                Suggestion("see wombat-docs for more information") 
            },
            std::nullopt
        );
    }

    // Given file is a valid wombat-source-file
    file_perms = fs::status(file).permissions();

    //! not-allowed to read from file.
    if(!can_write()) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { "file permissions denied, canonot write to " + file.filename().generic_string() },
            { Suggestion("change permissions to allow reading") },
            std::nullopt
        );
    }

    // All validation were successful.
    return std::monostate{};
}