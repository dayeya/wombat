#include <iomanip>
#include <expected>

#include "diag.hpp"

void Renderer::render_header(const Diagnostic& diag) const {
    std::string_view color = (diag.level == Level::Critical) ? RED_BOLD : YELLOW_BOLD;
    std::cout << color << diag.level_to_str() << RESET << ": " << diag.message;
}

void Renderer::render_marker(const Diagnostic& diag) const {
    ASSERT(diag.label.has_value(), "cannot render the marker of a non-existing label.");

    const auto& label = diag.label.value();

    std::printf(
        "at %s:%d:%d\n",
        label.region.file_name.c_str(),
        label.region.humanized_line(),
        label.region.location.col
    );

    // force a non-empty region.
    ASSERT(!label.region.empty(), "cannot render the marker of an empty region.");

    const auto& [trimmed_line, offset] = left_trim(label.region.source_code);

    // render the region (one source code line) where the error happened.
    std::printf(" |\n");
    std::printf("[%s%d%s]", CYAN_BOLD, label.region.humanized_line(), RESET); 
    std::printf("%s\n", trimmed_line.c_str());
    std::printf(" |");

    int caret_position = std::max(0, label.region.humanized_line() - offset);
    std::string caret = std::string(caret_position, ' ') + "^";
    
    std::printf("%s %s", caret.c_str(), label.text.c_str());
}

void Renderer::render_hint(const Diagnostic& diag) const {
    ASSERT(!diag.hint.empty(), "cannot pretty-render without an hint.");
    std::printf("%s~%s %s\n", GREEN_BOLD, RESET, diag.hint.c_str());
}

void Renderer::render_pretty(const Diagnostic& diag) const {
    render_header(diag);
    render_marker(diag);
    render_hint(diag);
}

void Renderer::render_short(const Diagnostic& diag) const {
    render_header(diag);
    render_hint(diag);
}