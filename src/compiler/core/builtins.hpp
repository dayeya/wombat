#ifndef BUILTIN_HPP_
#define BUILTIN_HPP_

#include <string>
#include <vector>

struct Builtin {
    std::string ident;

    Builtin(std::string&& f) : ident{std::move(f)} {}
};

const std::vector<Builtin> BUILTINS = {
    Builtin{"putchar"},
    Builtin{"putnum"}
};

#endif // BUILTIN_HPP_