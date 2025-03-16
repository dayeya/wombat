#ifndef AST_HPP_
#define AST_HPP_

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include "expr.hpp"
#include "visit.hpp"

enum class NodeType {
    Expr,
    Stmt
};

class AstNode {
public:
    explicit AstNode(NodeType t) : ty(t) {}
    virtual ~AstNode() = default;

    AstNode(AstNode&&) noexcept = default;
    AstNode& operator=(AstNode&&) noexcept = default;

    NodeType type() const { 
        return ty; 
    }

    // Pretty-print a node to std::out.
    virtual std::string pretty_print(int ident) = 0;

    virtual void accept(Visitor& visitor) = 0;
    

private:
    NodeType ty;
};

class ValueNode : public AstNode {
public:
    explicit ValueNode() : AstNode(NodeType::Expr), inner(std::nullopt) {}
    explicit ValueNode(Expr::Value expr) : AstNode(NodeType::Expr), inner(std::move(expr)) {}

    inline bool expr_exists() const { 
        return inner.has_value(); 
    }

    bool match(LiteralKind lit) const {
        return expr_exists() && inner->lit_kind == lit;
    }

    std::string pretty_print(int ident = 0) override {
        if (!inner.has_value()) {
            WOMBAT_ASSERT(false, "VALUE_NODE WITHOUT EXPR");
        }
        
        std::string top_level_ident(ident * 2, ' ');
        std::string children_level_ident((ident + 1) * 2, ' ');

        std::stringstream out;
        
        out << top_level_ident << "ValueNode: " << "\n"
            << children_level_ident << "kind: " << lit_kind_to_str(inner->lit_kind) << "\n"
            << children_level_ident << "value: " << inner->val << "\n";
    
        return out.str();
    }

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

private:
    Option<Expr::Value> inner;
};

class BinOpNode : public AstNode {
public:
    explicit BinOpNode(
        BinaryOperator op_kind,
        Ptr<AstNode> lhs, 
        Ptr<AstNode> rhs
    ) : AstNode(NodeType::Expr), op(op_kind), left(std::move(lhs)), right(std::move(rhs)) {} 

    bool match(BinaryOperator op) const { return op == op; }

    std::string pretty_print(int ident = 0) override {
        std::stringstream out;
        
        std::string top_level_ident(ident * 2, ' ');
        std::string children_level_ident((ident + 1) * 2, ' ');

        out << top_level_ident << "BinaryNode: " << "\n"
            << children_level_ident << "op: " << bin_op_kind_to_str(op) << "\n"
            << children_level_ident << "left: " << "\n" << left->pretty_print(ident + 2)
            << children_level_ident << "right: " << "\n" << right->pretty_print(ident + 2);
    
        return out.str();
    }
    

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

private:
    BinaryOperator op;
    Ptr<AstNode> left;
    Ptr<AstNode> right;
};

class AST {
public:
    explicit AST(Ptr<AstNode> r = nullptr) : root(std::move(r)) {}

    void traverse(Visitor& visitor) {        
        if (!root) {
            return;
        }
        root->accept(visitor);
    }
    
private:
    Ptr<AstNode> root;
};

#endif // AST_HPP_