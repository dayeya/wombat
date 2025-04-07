#ifndef DIAGNOSTIC_HPP_
#define DIAGNOSTIC_HPP_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>
#include <vector>
#include <string>
#include <expected>

#include "common.hpp"
#include "str.hpp"
#include "token.hpp"

CONST char* RED_BOLD     = "\033[1;31m";
CONST char* GREEN_BOLD   = "\033[1;32m";
CONST char* YELLOW_BOLD  = "\033[1;33m";
CONST char* BLUE_BOLD    = "\033[1;34m";
CONST char* CYAN_BOLD    = "\033[1;36m";
CONST char* RESET        = "\033[0m";

// Severity levels
enum class Level: int { Critical, Warning };

struct Region {
    std::string file_name;
    std::string source_code;
    Tokenizer::Location location;

    Region(std::string_view file, int line, int col, std::string source_line) 
        : file_name(file), location(line, col), source_code(std::move(source_line)) {}

    inline bool empty() const {
        return source_code.empty();
    }

    inline int humanized_line() const {
        return location.line + 1;
    }
};

struct Label {
    Region region;
    std::string text;

    Label(std::string text, Region region) 
        : text(std::move(text)), region(std::move(region)) {}
};

struct Diagnostic {
    Level level;
    std::string message;
    std::string hint;
    Option<Label> label;

    Diagnostic(Level lvl, std::string msg, std::string hint, Option<Label> lbl = std::nullopt) 
        : level(lvl), message(std::move(msg)), hint(std::move(hint)), label(std::move(lbl)) {}

    std::string level_to_str() const noexcept {
        return (level == Level::Critical) ? "critical" : "warning";
    }
};

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void render_short(const Diagnostic& diag) const;
    void render_pretty(const Diagnostic& diag) const;

private:
    void render_header(const Diagnostic& diag) const;
    void render_marker(const Diagnostic& diag) const;
    void render_hint(const Diagnostic& diag) const;
};

class Diagnostics {
    Renderer renderer;
    std::vector<Diagnostic> diagnostics;

public:
    explicit Diagnostics(size_t max_capacity) {
        diagnostics.reserve(max_capacity);
    }

    void push(const Diagnostic& diag) {
        diagnostics.push_back(diag);
    }
};

#endif // DIAGNOSTIC_HPP_