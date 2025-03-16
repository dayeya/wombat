#include "test_help.hpp"

Ptr<Token> build_token(std::string v, TokenKind k) {
    Ptr<Token> tok = std::make_unique<Token>();
    tok->fill_with(v, k);
    return tok;
}