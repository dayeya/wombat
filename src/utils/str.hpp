#ifndef STR_HPP_
#define STR_HPP_

#include <iostream>
#include <algorithm>

bool is_alnum(char c);
bool is_digit(char c);
bool is_alpha(char c);
bool is_symbol(char c);

/// Removes leading whitespace characters from the string and counts how many bytes it removed.
std::pair<std::string, int> left_trim(const std::string& str);

#endif // STR_HPP_
