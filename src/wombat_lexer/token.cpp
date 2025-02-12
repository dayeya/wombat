#include <iostream>
#include "token.hpp"

auto TokenStream::numOfTokens() -> std::size_t {
  return m_tokens.size();
} 

auto TokenStream::pushToken(const Token& wombat_token) -> bool {
  auto self_size = numOfTokens();
  m_tokens.insert(m_tokens.end(), wombat_token);
  return (self_size + 1) == numOfTokens();
}