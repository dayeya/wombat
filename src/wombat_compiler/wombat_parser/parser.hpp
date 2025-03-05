#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "token.hpp"

using AST = void;

class Parser {
public: 
    auto parse_to_AST() -> AST; 
private:
    std::unique_ptr<LazyTokenStream> tr;
};

#endif // PARSER_HPP_