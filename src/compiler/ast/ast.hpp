#ifndef AST_HPP_
#define AST_HPP_

#include "visit.hpp"
#include "node.hpp"

class AST {
public:
    explicit AST(Ptr<Node> r = nullptr) : root(std::move(r)) {}

    void set_entry_point(Ptr<Node>& entry) {
        root = std::move(entry);
    }

    void traverse(PPVisitor& visitor) {        
        if (!root) {
            return;
        }
        root->accept(visitor);
    }
    
private:
    Ptr<Node> root;
};

#endif // AST_HPP_