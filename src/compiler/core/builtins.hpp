#ifndef BUILTIN_HPP_
#define BUILTIN_HPP_

#include <string>
#include <vector>

struct Builtin {
    std::string ident;
    std::string signature;

    Builtin(std::string&& f, std::string&& s) 
        : ident{std::move(f)}, signature{std::move(s)} {}
};

const std::vector<Builtin> BUILTINS = {
    Builtin{"putchar",  "fn free putchar(_1: char);"},
    Builtin{"putnum",   "fn free putnum(_1: int);"},
    Builtin{"quit",     "fn free quit(_1: int);"},
    Builtin{"assert",   "fn free assert(_1: bool);"}
};

Option<SymFunction> sig_to_sym(const std::string& wombat_sig);

#endif // BUILTIN_HPP_