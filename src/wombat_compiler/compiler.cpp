#include "compiler.hpp"
#include "lex.hpp"

auto Compiler::start_lexing(std::string path) -> void {
    Lexer lexer = Lexer { path };
    TokenStream token_stream = lexer.lex_source();
    for(const auto& token : token_stream.tokens()) {
        lexer.output_token(token);
    }
}