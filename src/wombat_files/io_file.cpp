#include "io_file.hpp"
#include "diagnostic.hpp"

#include <expected>

auto IoFile::can_read() -> bool {
    return (file_perms & fs::perms::owner_read) != fs::perms::none;
}

auto IoFile::can_write() -> bool {
    return (file_perms & fs::perms::owner_write) != fs::perms::none;
}

auto IoFile::validate(
    const std::string& extension, 
    bool for_read, 
    bool for_write
) -> std::expected<std::monostate, Diagnostic> {
    if(!fs::exists(file)) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            file.generic_string() + " does not exist", 
            "provide an existing file location", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    if(fs::is_directory(file)) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            file.generic_string() + " is a directory", 
            "provide an existing, valid .wo source file", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    if(file.extension() != extension) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            file.extension().generic_string() + " is not the expected extension, use " + extension, 
            "see wombat-docs for more information", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    //! file was empty.
    //! Can occur if component-validation on file failed.  
    if(file.empty()) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            "cannot operate on an empty path", 
            "see wombat-docs for more information", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    // Given file is a valid wombat-source-file
    file_perms = fs::status(file).permissions();

    //! not-allowed to read from file.
    if(for_read && !can_read()) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            "file permissions denied, canonot read from " + file.filename().generic_string(), 
            "change permissions to allow reading", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    //! not-allowed to write to file.
    if(for_read && !can_write()) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            "file permissions denied, canonot write from " + file.filename().generic_string(), 
            "change permissions to allow writing", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    // All validation were successful.
    return std::monostate{};
}