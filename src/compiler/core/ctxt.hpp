#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

#include "ast.hpp"
#include "lex.hpp"

struct Context {
    LazyTokenStream program_tokens;
    AST program_ast;

    Context() : program_tokens(), program_ast() {}
};

#endif // CONTEXT_HPP_