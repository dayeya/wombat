#ifndef AST_HPP_
#define AST_HPP_

#include <iostream>

#include "expr.hpp"

using std::unique_ptr, std::optional;

enum class NodeType {
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

    explicit AstNode(
        NodeType t, 
        unique_ptr<AstNode> lhs = nullptr, 
        unique_ptr<AstNode> rhs = nullptr
    ) : ty(t), left(std::move(lhs)), right(std::move(rhs)) {}

    virtual ~AstNode() = default;

    AstNode(const AstNode&) = delete;
    AstNode& operator=(const AstNode&) = delete;

    AstNode(AstNode&&) noexcept = default;
    AstNode& operator=(AstNode&&) noexcept = default;

private:
    NodeType ty;
};

//! A node that represents an expression.
//! *in most cases* It is a leaf.
class ExprNode : public AstNode {
public:
    explicit ExprNode() : AstNode(NodeType::Expr), inner(std::nullopt) {}
    explicit ExprNode(Expr expr) : AstNode(NodeType::Expr), inner(std::move(expr)) {}

    ~ExprNode() override = default;

    bool expr_exists() {
        return inner.has_value();
    }

    bool match(ExprKind ek) {
        if(!expr_exists()) return false;
        return inner->expr_kind == ek;
    }

private:
    optional<Expr> inner;
};

//! A binary operation node, *must have children*; 
class BinOpNode : public AstNode {
public:
    explicit BinOpNode(
        BinOp op_kind,
        unique_ptr<AstNode> lhs, 
        unique_ptr<AstNode> rhs
    ) : AstNode(NodeType::Expr, std::move(lhs), std::move(rhs)), operation(op_kind) {} 

    bool match(BinOp op) {
        return operation == op;
    }

private:
    BinOp operation;
};

class AST {
public:
    unique_ptr<AstNode> root;

    explicit AST(unique_ptr<AstNode> r = nullptr) : root(std::move(r)) {};

    ~AST() = default;
};

#endif // AST_HPP_