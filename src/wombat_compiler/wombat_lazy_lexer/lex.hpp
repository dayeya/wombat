#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cassert>
#include <vector>

#include "token.hpp"

using std::unique_ptr, std::shared_ptr;

namespace fs = std::filesystem;

struct Lexer {
    std::stringstream source; 
    std::string file_name;
    char m_cur;
    int m_line, m_col;
    
    Lexer(std::string s) {
	    std::ifstream ifs{s};
        if(ifs.is_open()) {
            file_name = s;
            source << ifs.rdbuf();
        } else {
	        std::cout << "[LEXER::ERROR] could not start lexer, file opening failed" << std::endl;
	    }
    };

    bool lexer_is_alpha(char c);
    bool lexer_is_alnum(char c);
    bool lexer_is_digit(char c);
    bool lexer_is_symbol(char c);
    bool lexer_is_new_line(char c);
    bool lexer_is_readable(std::string iden);

    void skip_whitespaces();
    void output_token(const Token& token);
    void assign_token(Token* token, std::string text, TokenKind kind);

    auto peek_next() -> int;
    auto advance_cursor() -> char;
    auto advance_with_token(const Token& token);
    auto kind_to_str(const TokenKind& kind) -> std::string;

    void lex_eof(Token* token);
    void lex_foreign(Token* token, char ch);
    void lex_line_comment(Token* token);
    auto lex_word(Token* token) -> std::expected<Token*, Diagnostic>;
    auto lex_literal(Token* token) -> std::expected<Token*, Diagnostic>;
    auto lex_symbol(Token* token) -> std::expected<Token*, Diagnostic>;
    auto lexer_next_token(TokenStream* token_stream, Token* token) -> std::expected<Token*, Diagnostic>;
    auto lex_source() -> TokenStream;
};

struct LazyLexer {
    Lexer lexer;

    auto next_token() -> Token;  
};

#endif // LEXER_HPP_