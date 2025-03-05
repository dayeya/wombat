#ifndef TEST_HELP_HPP_
#define TEST_HELP_HPP_

#include "token.hpp"

unique_ptr<Token> build_token(std::string v, TokenKind k);

#endif // TEST_HELP_HPP_