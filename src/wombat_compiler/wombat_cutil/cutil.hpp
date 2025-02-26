#ifndef C_UTILS_H_
#define C_UTILS_H_

#include <iostream>
#include <algorithm>

namespace CharUtils {
    bool is_alnum(char c);
    bool is_digit(char c);
    bool is_alpha(char c);
    bool is_symbol(char c);

    std::pair<std::string, int> left_trim(const std::string& str);
}

#endif // C_UTILS_H_