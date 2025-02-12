#include "emitter.hpp"

auto Emitter::emit_diagnostic(const Diagnostic& diag) const -> void {
    diag.print_header();
    diag.print_location();
    diag.print_message();
    diag.print_suggestion();
}
