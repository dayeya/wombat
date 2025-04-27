#ifndef AST_HPP_
#define AST_HPP_

#include "visit.hpp"
#include "node.hpp"

class AST {
public:
    explicit AST() : functions() {}

    bool contains_entry_point() const {
        for(const auto& fn : functions) {
            if(fn->header->header->ident.matches("main")) {
                return true;
            }
        }
    }

    void push_function(Ptr<FnNode>&& fn) {
        functions.push_back(std::move(fn));
    }

    void traverse(PPVisitor& visitor) {
        for(const auto& fn : functions) {
            fn->accept(visitor);
        }
    }
    
private:
    std::vector<Ptr<FnNode>> functions;
};

#endif // AST_HPP_