#ifndef ENV_HPP_
#define ENV_HPP_

#include "common.hpp"

CONST std::string COMPILER_NAME = "wmc";
CONST std::string VERSION_NUMBER = "v0.1";

CONST char* signature() {
    return (COMPILER_NAME + " " + VERSION_NUMBER).c_str();
}

#endif // ENV_HPP_