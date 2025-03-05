#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <iostream>
#include <memory>
#include <filesystem>
#include <string>
#include <vector>
#include <vector>
#include <expected>
#include <sstream>

#include "token.hpp"
#include "diagnostic.hpp"

namespace fs = std::filesystem;

struct Cursor {
    std::string file_name;
    std::vector<std::string> source;
    char current = 0;
    int current_line = 0;
    int current_col = 0;
    int total_lines = 0;
    bool eof = false;

    explicit Cursor(std::string filename) 
        : file_name(std::move(filename)), source() {}

    char advance_self() {
        if (current_col == source[current_line].size()) {
            if (current_line + 1 < total_lines) {
                current_line++;
                current_col = 0;
                current = '\n';
                return current;
            } else {
                eof = true;
                return '\0';
            }
        } else {
            current = source[current_line][current_col++];
            return current;
        }
    }

    //! Peeks the next character safely.
    char peek_next() {
        if (current_col < source[current_line].size()) {
            return source[current_line][current_col];
        }
        
        if (current_line + 1 < total_lines && !source[current_line + 1].empty()) {
            return source[current_line + 1][0];
        }
        return '\0';
    }

    void skip_whitespace() {
        while (
            !reached_eof() && 
            std::isspace(static_cast<unsigned char>(peek_next()))
        ) {
            advance_self();
        }
    }

    bool reached_new_line() const {
        return current == '\n' || current == '\r';
    }

    bool reached_eof() const {
        return eof;
    }

    std::vector<std::string> one_lined_region(int line = -1) {
        std::vector<std::string> region(1, "");

        int target_line = (line == -1) ? current_line : line;

        if (target_line >= 0 && target_line < total_lines) {
            region[0] = source[target_line];
        }

        return region;
    }

    std::vector<std::string> multi_lined_region(int start_line, int end_line) {
        start_line = std::max(0, start_line);
        end_line = std::min(total_lines - 1, end_line);
    
        if (start_line >= total_lines || start_line > end_line) return {};
    
        return {source.begin() + start_line, source.begin() + end_line + 1};
    }

    //! Returns the human-readable (1-based) line number.
    int humanized_current_line() const {
        return current_line + 1;
    }
};


class Lexer {
public:
    std::vector<Diagnostic> diagnostics;

    explicit Lexer(std::string native_path) 
        : m_cursor(native_path) {}

    bool open_and_populate_cursor();
    LazyTokenStream lex_source();

private:
    Cursor m_cursor;

    inline char advance_cursor() { return m_cursor.advance_self(); }

    void lex_eof(unique_ptr<Token>& token);
    void lex_foreign(unique_ptr<Token>& token, char ch);
    void lex_line_comment(unique_ptr<Token>& token);

    void lex_word(unique_ptr<Token>& token_stream);
    void lex_literal(unique_ptr<Token>& token_stream);
    void lex_symbol(unique_ptr<Token>& token_stream);
    void next_token(LazyTokenStream& token_stream);

    void register_warning_diagnostic_pretty(
        std::string message, 
        std::string hint,
        std::vector<Label> labels
    ) {
        diagnostics.emplace_back(Diagnostic(
            Level::Warning, 
            Phase::Lexer, 
            message, 
            hint, 
            labels
        ));
    }

    void register_critical_diagnostic_pretty(
        std::string message, 
        std::string hint,
        std::vector<Label> labels
    ) {
        diagnostics.emplace_back(Diagnostic(
            Level::Critical, 
            Phase::Lexer, 
            message, 
            hint, 
            labels
        ));
    }

    void register_critical_diagnostic_short(std::string message, std::string hint) {
        diagnostics.emplace_back(Diagnostic(
            Level::Critical, 
            Phase::Lexer, 
            message, 
            hint, 
            {}
        ));
    }
      
    void register_warning_diagnostic_short(std::string message, std::string hint) {
        diagnostics.emplace_back(Diagnostic(
            Level::Warning, 
            Phase::Lexer, 
            message, 
            hint, 
            {}
        ));
    }
};

#endif // LEXER_HPP_