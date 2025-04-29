#ifndef AST_HPP_
#define AST_HPP_

#include "node.hpp"
#include "pp_visitor.hpp"

struct AST {
    using FnContainer = std::vector<Ptr<FnNode>>;

    // Since `Wombat` is a functional programming language,
    // An AST is built from multiple functions.
    FnContainer functions;

    AST() : functions{} {}

    void push_function(Ptr<FnNode>&& fn) {
        functions.push_back(std::move(fn));
    }

    void traverse(PPVisitor& visitor) {
        for(const auto& fn : functions) {
            fn->accept(visitor);
        }
    }
};

#endif // AST_HPP_