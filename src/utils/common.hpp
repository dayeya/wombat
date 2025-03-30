#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <cstdio>
#include <iostream>
#include <string>
#include <source_location>

constexpr const char* ERROR_PREFIX = "ERROR";
constexpr const char* WARNING_PREFIX = "WARNING";
constexpr const char* FATAL_ERROR_PREFIX = "FATAL_ERROR";
constexpr const char* ASSERT_PREFIX = "ASSERTION";
constexpr const char* TODO_PREFIX = "TODO";
constexpr const char* UNREACHABLE_PREFIX = "UNREACHABLE";

inline void ASSERT(
    bool cond, 
    const std::string& msg, 
    const std::string& prefix = ASSERT_PREFIX,
    std::source_location loc = std::source_location::current()
) {
    if (!cond) {
        std::fprintf(stderr, "[%s] ", prefix.c_str());
        std::fprintf(stderr, "%s:%d:%d ", loc.file_name(), loc.line(), loc.column());
        std::fprintf(stderr, "- %s\n", msg.c_str());
        std::exit(EXIT_FAILURE);
    }
}

inline void TODO(const std::string& todo, const std::string& version = "", const std::string& from = "") {
    if (!version.empty()) {
        std::printf("VERSION: [%s] ", version.c_str());
    }
    if (!from.empty()) {
        std::printf("FROM: [%s] ", from.c_str());
    }
    std::printf("[TODO] -- %s\n", todo.c_str());
}

inline void UNREACHABLE(const char* msg="") {
    ASSERT(false, msg, UNREACHABLE_PREFIX);
}

#endif // COMMON_HPP_