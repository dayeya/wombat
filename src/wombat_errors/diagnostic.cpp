#include <iomanip>
#include <expected>

#include "diagnostic.hpp"

auto unexpected_diagnostic_from(
    DiagnosticKind kind,
    std::vector<std::string> messages,
    std::vector<Suggestion> suggestions,
    std::optional<CodeLocation> code_loc
) -> std::unexpected<Diagnostic> {
    auto diag = Diagnostic(kind, messages, suggestions, code_loc);  
    return std::unexpected(diag);
};

auto Suggestion::suggest() const -> void {
    std::cout << "maybe! " << suggestion_ << ".\n";
}

auto Diagnostic::print_header() const -> void {
    std::cout << "Diagnostic [" << make_readable_kind(kind_) << "]\n"; 
}

auto Diagnostic::print_location() const -> void {
    if(!code_loc_.has_value()) return;
    std::cout << code_loc_->file_name << ":" << code_loc_->line << ":" << code_loc_->col << ":\n";
}

auto Diagnostic::print_message() const -> void {
    int index = 1;
    for (const auto& msg : messages_) {
        std::cout << " |\n";
        std::cout << "[\033[1;36m" << index << "\033[0m] " << msg << ".\n";
        std::cout << " |\n";
        ++index;
    }
}
auto Diagnostic::print_suggestion() const -> void {
    for(const auto& suggestion : suggestions_) {
        suggestion.suggest();
    }
}