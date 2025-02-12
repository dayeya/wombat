#include <iomanip>

#include "diagnostic.hpp"

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
        std::cout << "[\033[1;36m" << index << "\033[0m] " << msg << ".\n";
        ++index;
    }
}
auto Diagnostic::print_suggestion() const -> void {
    for(const auto& suggestion : suggestions_) {
        suggestion.suggest();
    }
}