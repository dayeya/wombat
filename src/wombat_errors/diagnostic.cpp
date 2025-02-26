#include <iomanip>
#include <expected>

#include "diagnostic.hpp"

constexpr auto Diagnostic::phase_to_str() const -> std::string {
    switch (phase) {
        case Phase::Precomp:   return "Pre-compilation";
        case Phase::Lexer: return "Lexical-analysis";
        case Phase::Parser:    return "Syntax-and-semantic analysis";
        case Phase::CodeGen:   return "Assembly-code-generation";
        case Phase::Optimize:  return "Code-optimization";
        default: 
            return "Unknown";
    }
}

constexpr auto Diagnostic::level_to_str() const -> std::string {
    switch (level) {
        case Level::Warning:  return "Warning";
        case Level::Critical: return "Critical";
        case Level::Help:     return "Help";
        default: return "Unknown";
    }
}

void Renderer::render_pretty_print(const Diagnostic& diag) const {
    auto level = diag.level_to_str();
    auto phase = diag.phase_to_str();
    auto header = Header { level, diag.message }; 

    std::cout << header.format().str() << std::endl;

    if(!diag.labels.empty()) {
        //! NOT IMPLEMENTED.
        std::cout << "<Has labels, not implemented for now...>" << std::endl;
    }

    if(!diag.hint.empty()) {
        std::cout << "~ " << diag.hint << ".\n";
    }
}

//!
//! ERROR FORMATTING
// Error: unterminated string 
// at main.cpp:5:21
//  |
// [5] let x = "daniel"name"; 
//  |                  ^^^^^
//  |                  invalid syntax
//  ~
//
//
// Short form.
//
// Error: not enough arguments provided                  
// ~ use ` ./wombat --help ` for more information

//!
//!
// Warning: Unused variable `z`
// test.wb:7:8
//  |
// [7] let! z = 42;
//  |       ^ declared but never used
//  |
//  ~ remove `z` or prefix it with `_` to silence this warning.