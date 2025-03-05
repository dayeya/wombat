#include "test_help.hpp"

unique_ptr<Token> build_token(std::string v, TokenKind k) {
    unique_ptr<Token> tok = std::make_unique<Token>();
    tok->fill_with(v, k);
    return tok;
}