#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <vector>
#include <expected>
#include <sstream>

#include "token.hpp"
#include "diag.hpp"

using Tokenizer::Token;
using Tokenizer::TokenKind;
using Tokenizer::Location;
using Tokenizer::LazyTokenStream;

struct SourceCursor {
    std::string file_name;
    std::vector<std::string> source;
    Location cur_loc = Location::Singularity();
    char current = 0;
    int total_lines = 0;
    bool eof = false;

    explicit SourceCursor(std::string filename) 
        : file_name(std::move(filename)), source() {}

    // Advances the cursor, using pre-increments.
    // This is makes peeking easier just by using `cur_loc.col` instead of incrementing once again.
    char advance_self() {
        if (cur_loc.col == source[cur_loc.line].size()) {
            if (cur_loc.line + 1 < total_lines) {
                cur_loc.line++;
                cur_loc.col = 0;
                current = '\n';
                return current;
            } else {
                eof = true;
                return '\0';
            }
        } else {
            current = source[cur_loc.line][cur_loc.col++];
            return current;
        }
    }

    // Peeks the next character safely.
    char peek_next(int step_size = 0) {
        if (cur_loc.col + step_size < source[cur_loc.line].size()) {
            return source[cur_loc.line][cur_loc.col + step_size];
        }
        if (cur_loc.line + 1 < total_lines) {
            if (step_size < source[cur_loc.line + 1].size()) {
                return source[cur_loc.line + 1][step_size];
            }
        }
        return '\0';
    }

    // Rewinds the cursor `step_size` bytes back.
    void rewind(int step_size) {
        ASSERT(step_size > 0, "Cannot rewind lexer.SourceCursor with a negative step_size");
        if(cur_loc.col >= step_size) {
            cur_loc.col -= step_size;
        } else {
            cur_loc.col = 0;
        }
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

        int target_line = (line == -1) ? cur_loc.line : line;

        if (target_line >= 0 && target_line < total_lines) {
            region[0] = source[target_line];
        }

        return region;
    }
};


class Lexer {
public:
    Diagnostics diags;

    static CONST int LEXER_MAX_DIAGS = 10;

    explicit Lexer(std::string native_path) 
        : m_cursor(native_path), tok(std::make_unique<Token>()), diags(Lexer::LEXER_MAX_DIAGS) {}

    SourceCursor& get_cursor() {
        return m_cursor;
    }    

    bool open_and_populate_cursor();
    
    LazyTokenStream lex_source();

private:
    Ptr<Token> tok;
    SourceCursor m_cursor;

    inline char advance_cursor() { 
        return m_cursor.advance_self(); 
    }

    void lex_eof();
    void lex_foreign(char ch);
    void lex_line_comment();

    void lex_word();
    void lex_literal();
    void lex_symbol();
    void next_token(LazyTokenStream& token_stream);

    inline void push_diagnostic(const Diagnostic& diag) { diags.push(diag); }
};

#endif // LEXER_HPP_