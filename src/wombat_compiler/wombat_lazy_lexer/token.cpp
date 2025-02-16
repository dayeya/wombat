#include <iostream>

#include "token.hpp"

auto TokenStream::num_of_tokens() -> std::size_t {
  return m_tokens.size();
} 

auto TokenStream::push_token(const Token& wombat_token) -> bool {
  auto self_size = num_of_tokens();
  m_tokens.insert(m_tokens.end(), wombat_token);
  return (self_size + 1) == num_of_tokens();
}