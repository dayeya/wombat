#ifndef AST_HPP_
#define AST_HPP_

#include <iostream>

#include "expr.hpp"

using std::unique_ptr, std::optional;

enum class NodeKind {
    Expr,
    Stmt
}; 

// Base class for all AST nodes
class AstNode {
public:
    std::unique_ptr<AstNode> left;
    std::unique_ptr<AstNode> right;

    AstNode()
        : left(nullptr), 
          right(nullptr) {}

    virtual ~AstNode() = default;
};

class ExprNode : AstNode {
public:
    optional<Expr> inner;

    ExprNode() : AstNode(), inner(std::nullopt) {}
    ExprNode(Expr expr) : AstNode(), inner(std::move(expr)) {}
};

#endif // AST_HPP_