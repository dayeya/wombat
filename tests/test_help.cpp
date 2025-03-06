#include "test_help.hpp"

SmartPtr<Token> build_token(std::string v, TokenKind k) {
    SmartPtr<Token> tok = std::make_unique<Token>();
    tok->fill_with(v, k);
    return tok;
}