#include <iomanip>
#include <expected>

#include "diagnostic.hpp"

constexpr auto Diagnostic::phase_to_str() const -> std::string {
    switch (phase) {
        case Phase::Precomp:   return "precompilation";
        case Phase::Lexer:     return "lexical analysis";
        case Phase::Parser:    return "syntax and semantic analysis";
        case Phase::CodeGen:   return "code generation";
        case Phase::Optimize:  return "Code optimization";
        default: return "Unknown";
    }
}

constexpr auto Diagnostic::level_to_str() const -> std::string {
    switch (level) {
        case Level::Warning:  return "warning";
        case Level::Critical: return "critical";
        case Level::Help:     return "help";
        default: return "unknown";
    }
}

void Renderer::render_pretty_print(const Diagnostic& diag) const {
    auto level = diag.level_to_str();
    auto phase = diag.phase_to_str();
    auto header = Header { level, diag.message }; 

    std::cout << header.format().str() << "\n";

    if(!diag.labels.empty()) {
        for(const auto& label : diag.labels) {
            for(const auto& reg : label.regions) {
                auto marker = Marker { reg.location.first, reg, label.text };
                std::cout << marker.format().str() << "\n";
            }
        }
    }

    std::cout << " ~ ";

    if(!diag.hint.empty()) {
        std::cout << diag.hint << "\n";
    } else { 
        std::cout << "\n";
    }

    std::cout << "\n";
}

void Renderer::render_short(const Diagnostic& diag) const {
    auto level = diag.level_to_str();
    auto phase = diag.phase_to_str();
    auto header = Header { level, diag.message }; 

    std::cout << header.format().str() << "\n";
    std::cout << "~ ";

    if(!diag.hint.empty()) {
        std::cout << diag.hint << "\n";
    } else { 
        std::cout << "\n";
    }

    std::cout << "\n";
}