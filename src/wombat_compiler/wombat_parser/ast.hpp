#ifndef AST_HPP_
#define AST_HPP_

#include <iostream>

#include "expr.hpp"

using std::unique_ptr, std::optional;

enum class NodeKind {
    //! An expression node.
    //! See [expr.hpp -> enum ExprKind]
    //! 
    //! A node of this kind can have left and right children.
    Expr,

    //! A statement node.
    //! See [stmt.hpp -> StmtKind]
    //!
    //! A node of this kind will hold the statement kind within the root.
    //! *must have children*
    Stmt
}; 

class AstNode {
public:
    unique_ptr<AstNode> left;
    unique_ptr<AstNode> right;

    AstNode(unique_ptr<AstNode> lhs, unique_ptr<AstNode> rhs)
        : left(std::move(lhs)), 
          right(std::move(rhs)) {}

    virtual ~AstNode() = default;
};


//! A node that represents an expression.
//! *in most cases* It is a leaf.
class ExprNode : public AstNode {
public:
    optional<Expr> inner;

    ExprNode() : AstNode(nullptr, nullptr), inner(std::nullopt) {}
    ExprNode(Expr expr) : AstNode(nullptr, nullptr), inner(std::move(expr)) {}   
};

//! A binary operation node, *must have children*; 
class BinOpNode : public AstNode {
public:
    BinOp operation;

    BinOpNode(
        BinOp op_kind,
        unique_ptr<AstNode> lhs, 
        unique_ptr<AstNode> rhs
    ) : AstNode(std::move(lhs), std::move(rhs)), operation(op_kind) {} 
};   

#endif // AST_HPP_