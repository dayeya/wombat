#ifndef STR_CH_HPP_
#define STR_CH_HPP_

#include <iostream>
#include <algorithm>

/// Checks if the given character is alphanumeric (letters or digits).
bool is_alnum(char c);

/// Checks if the given character is a digit.
bool is_digit(char c);

/// Checks if the given character is an alphabetic letter (either uppercase or lowercase).
bool is_alpha(char c);

/// Checks if the given character is a symbol (non-alphanumeric character).
bool is_symbol(char c);

/// Removes leading whitespace characters from the string and counts how many bytes it removed.
std::pair<std::string, int> left_trim(const std::string& str);

#endif // STR_CH_HPP_
