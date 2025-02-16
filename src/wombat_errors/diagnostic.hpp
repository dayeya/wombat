#ifndef DIAGNOSTIC_HPP_
#define DIAGNOSTIC_HPP_

#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <expected>

enum class DiagnosticPhase { 
    PRECOMP,    /// Pre-compilation phase.
    LEXING,     /// Lexical analysis phase.
    PARSING,    /// Syntax and semantic analysis phase.
    CODEGEN,    /// Assembly code generation phase.
    OPTIMIZE,   /// Code optimization phase.
    VALIDATION  /// Validation phase, such as file permission checks.
};

enum class DiagnosticKind {
    Info,       /// Information.
    Warning,    /// Warning.
    Critical,   /// Critical.
    Help        /// Help.
};

/**
 * @brief Converts a `DiagnosticPhase` enumeration value to a stack-allocated string representation.
 * 
 * This function provides a readable string representation of each compilation phase.
 * @param p Phase to convert.
 * @returns stack-allocated readable string of 'p'.
 */
constexpr auto make_readable_phase(DiagnosticPhase p) -> const char* {
    switch (p) {
        case DiagnosticPhase::PRECOMP:    return "Pre-compilation";
        case DiagnosticPhase::LEXING:     return "Lexical-analysis";
        case DiagnosticPhase::PARSING:    return "Syntax-and-semantic analysis";
        case DiagnosticPhase::CODEGEN:    return "Assembly-code-generation";
        case DiagnosticPhase::OPTIMIZE:   return "Code-optimization";
        case DiagnosticPhase::VALIDATION: return "Input-= Validation";
        default: return "Unknown";
    }
}

constexpr auto make_readable_kind(DiagnosticKind p) -> const char* {
    switch (p) {
        case DiagnosticKind::Info:     return "Information";
        case DiagnosticKind::Warning:  return "Warning";
        case DiagnosticKind::Critical: return "Critical";
        case DiagnosticKind::Help:     return "Help";
        default: return "Unknown";
    }
}

struct CodeLocation {
    std::string file_name;
    int line;
    int col;

    CodeLocation(const std::string& fn, int l, int c) : file_name(fn), line(l), col(c) {}
};

struct Suggestion {
    std::string suggestion_;

    Suggestion(std::string suggestion) : suggestion_(suggestion) {}

    auto suggest() const -> void;
};

struct Diagnostic {
    DiagnosticKind kind_;
    std::vector<std::string> messages_;
    std::vector<Suggestion> suggestions_;
    std::optional<CodeLocation> code_loc_;

    Diagnostic(
        DiagnosticKind kind,
        const std::vector<std::string>& message,
        const std::vector<Suggestion>& suggestions,
        std::optional<CodeLocation> code_loc
    )
        : kind_(kind), code_loc_(code_loc), messages_(message), suggestions_(suggestions) {}
    
    auto print_header() const -> void;
    auto print_location() const -> void;
    auto print_message() const -> void;
    auto print_suggestion() const -> void;
};

auto unexpected_diagnostic_from(
    DiagnosticKind kind,
    std::vector<std::string> messages,
    std::vector<Suggestion> suggestions,
    std::optional<CodeLocation> code_loc
) -> std::unexpected<Diagnostic>;

#endif // DIAGNOSTIC_HPP_