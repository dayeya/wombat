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
    NodeType ty;
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
};

//! A node that represents an expression.
//! *in most cases* It is a leaf.
class ExprNode : public AstNode {
public:
    optional<Expr> inner;

    explicit ExprNode() : AstNode(NodeType::Expr), inner(std::nullopt) {}
    explicit ExprNode(Expr expr) : AstNode(NodeType::Expr), inner(std::move(expr)) {}

    ~ExprNode() override = default;
};

//! A binary operation node, *must have children*; 
class BinOpNode : public AstNode {
public:
    BinOp operation;

    explicit BinOpNode(
        BinOp op_kind,
        unique_ptr<AstNode> lhs, 
        unique_ptr<AstNode> rhs
    ) : AstNode(NodeType::Expr, std::move(lhs), std::move(rhs)), operation(op_kind) {} 
};   

#endif // AST_HPP_