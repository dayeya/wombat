#include "cutil.hpp"

bool CharUtils::is_alnum(char c)  { 
    return std::isalnum(static_cast<unsigned char>(c));
}

bool CharUtils::is_digit(char c)  { 
    return std::isdigit(static_cast<unsigned char>(c));
}

bool CharUtils::is_alpha(char c)  { 
    return std::isalpha(static_cast<unsigned char>(c));
}

bool CharUtils::is_symbol(char c) { 
    return std::ispunct(static_cast<unsigned char>(c));
}

std::pair<std::string, int> CharUtils::left_trim(const std::string& str) {
    int whitespaces = 0;
    auto first_non_space = std::find_if(str.begin(), str.end(), [&whitespaces](unsigned char ch) {
        if (std::isspace(ch)) {
            whitespaces++;
            return false;
        }
        return true;
    });
    
    std::string s(first_non_space, str.end());
    return std::make_pair(s, whitespaces);
}