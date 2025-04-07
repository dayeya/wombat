#ifndef NODE_HPP_
#define NODE_HPP_

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include "expr.hpp"
#include "stmt.hpp"
#include "visit.hpp"

struct Node {
    virtual std::string pretty_print(int ident) = 0;
    virtual void accept(PPVisitor& visitor) = 0;
};

struct LiteralNode : public Node {
    Option<Expr::Literal> val;

    explicit LiteralNode() : Node(), val(std::nullopt) {}
    explicit LiteralNode(const Expr::Literal& val) : Node(), val(std::move(val)) {}

    inline bool empty() const { 
        return !val.has_value(); 
    }

    inline bool match(LiteralKind kind) const { 
        return !empty() && val->kind == kind; 
    }

    std::string pretty_print(int indent = 0) override {
        ASSERT(!empty(), "cannot pretty-print a literal without a value.");
        
        std::string children_level_ident((indent + 1) * 2, ' ');

        std::stringstream out;
        
        out << children_level_ident << "kind: " << meaning_from_literal_kind(val->kind) << "\n"
            << children_level_ident << "value: " << val->val << "\n";
    
        return out.str();
    }

    void accept(PPVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct BinOpNode : public Node {
    BinOpKind op;
    Ptr<Node> lhs;
    Ptr<Node> rhs;

    explicit BinOpNode(BinOpKind op_kind, Ptr<Node> lhs, Ptr<Node> rhs) 
        : Node(), op(op_kind), lhs(std::move(lhs)), rhs(std::move(rhs)) {} 

    inline bool match(BinOpKind op) const { 
        return op == op; 
    }

    std::string pretty_print(int ident = 0) override {
        std::stringstream out;
        
        std::string top_level_ident(ident * 2, ' ');
        std::string children_level_ident((ident + 1) * 2, ' ');

        out << top_level_ident << "BinaryNode: " << "\n"
            << children_level_ident << "op: " << Tokenizer::meaning_from_bin_op_kind(op) << "\n"
            << children_level_ident << "left: " << "\n" << lhs->pretty_print(ident + 2)
            << children_level_ident << "right: " << "\n" << rhs->pretty_print(ident + 2);
    
        return out.str();
    }

    void accept(PPVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct UnaryOpNode : public Node {
    UnOpKind op;
    Ptr<Node> lhs;

    explicit UnaryOpNode(UnOpKind op_kind, Ptr<Node> lhs) 
        : Node(), op(op_kind), lhs(std::move(lhs)) {} 

    bool match(BinOpKind op) const { return op == op; }

    std::string pretty_print(int ident = 0) override {
        std::stringstream out;
        
        std::string top_level_ident(ident * 2, ' ');
        std::string children_level_ident((ident + 1) * 2, ' ');

        out << top_level_ident << "UnaryNode: " << "\n"
            << children_level_ident << "op: " << meaning_from_un_op_kind(op) << "\n"
            << children_level_ident << "left: " << "\n" << lhs->pretty_print(ident + 2);
    
        return out.str();
    }
    
    void accept(PPVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct LocalDeclNode : public Node {
    Ptr<Declaration::Var> lhs;
    Ptr<Node> rhs;
    
    explicit LocalDeclNode()
        : Node(), 
          lhs(nullptr), 
          rhs(nullptr) {}

    explicit LocalDeclNode(
        Ptr<Declaration::Var> decl,
        Ptr<Node> rhs_initializer
    ) : lhs(std::move(decl)), rhs(std::move(rhs_initializer)) {}

    std::string pretty_print(int indent = 0) override {
        std::stringstream out;
        std::string top_level_indent(indent * 2, ' ');
        std::string children_level_indent((indent + 1) * 2, ' ');

        out << top_level_indent << "Local: " << "\n"
            << children_level_indent << "mutability: " << meaning_from_mutability(lhs->mut) << "\n"
            << children_level_indent << "type: " << meaning_from_primitive(lhs->type) << "\n"
            << children_level_indent << "ident: " << lhs->ident.as_str() << "\n";

        if(rhs != nullptr) {
            out << children_level_indent << "initializer: \n"
                << rhs->pretty_print(indent + 2);
        }
    
        return out.str();
    }
    
    void accept(PPVisitor& visitor) override {
        visitor.visit(*this);
    }
};

#endif // NODE_HPP_