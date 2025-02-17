#include "compiler.hpp"
#include "lex.hpp"

auto Compiler::start_lexing(std::string path) -> void {
    Lexer lexer = Lexer { path };
    Token cur_token;
    TokenStream token_stream;

    lexer.lexer_next_token(&token_stream, &cur_token);
    do {
        lexer.output_token(cur_token);
        lexer.lexer_next_token(&token_stream, &cur_token);
    } while(!cur_token.compare_kind(TokenKind::Eof));
}