#ifndef NODE_HPP_
#define NODE_HPP_

#include <iostream>
#include <optional>
#include <sstream>

#include "expr.hpp"
#include "stmt.hpp"
#include "visit.hpp"

inline std::string make_indent(int indent) {
  return std::string(indent * 2, ' ');
}

struct Node {
  virtual ~Node() = default;
  virtual void accept(PPVisitor& visitor) = 0;
  virtual std::string pretty_print(int indent) = 0;
};

struct StmtNode : virtual public Node {
  StmtNode() = default;
  ~StmtNode() = default;
};

struct ExprNode : virtual public Node {
  ExprNode() = default;
  ~ExprNode() = default;
};

struct LiteralNode : public ExprNode {
  Option<Expr::Literal> val;

  explicit LiteralNode() : ExprNode(), val(std::nullopt) {}
  explicit LiteralNode(const Expr::Literal& val) : ExprNode(), val(val) {}

  inline bool empty() const { return !val.has_value(); }

  inline bool match(LiteralKind kind) const {
    return !empty() && val->kind == kind;
  }

  std::string pretty_print(int indent = 0) override {
    ASSERT(!empty(), "cannot pretty-print a literal without a value.");
    std::stringstream out;

    out << make_indent(indent) << "LiteralNode:\n";
    out << make_indent(indent + 1) << "kind: " << meaning_from_literal_kind(val->kind) << "\n"
        << make_indent(indent + 1)  << "value: " << val->val << "\n";

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct BinOpNode : public ExprNode {
  BinOpKind op;
  Ptr<ExprNode> lhs;
  Ptr<ExprNode> rhs;

  explicit BinOpNode(BinOpKind op_kind, Ptr<ExprNode> lhs, Ptr<ExprNode> rhs)
    : ExprNode(), op(op_kind), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "BinaryNode:\n"
        << make_indent(indent + 1) << "op: " << Tokenizer::meaning_from_bin_op_kind(op) << "\n"
        << make_indent(indent + 1) << "left:\n" << lhs->pretty_print(indent + 2)
        << make_indent(indent + 1) << "right:\n" << rhs->pretty_print(indent + 2);

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct VarTerminalNode : public ExprNode {
  Expr::Identifier ident;

  explicit VarTerminalNode(Expr::Identifier&& ident)
    : ExprNode(), ident(std::move(ident)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "VarTerminal:\n"
        << make_indent(indent + 1) << "ident: " << ident.as_str() << "\n";

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct UnaryOpNode : public ExprNode {
  UnOpKind op;
  Ptr<ExprNode> lhs;

  explicit UnaryOpNode(UnOpKind op_kind, Ptr<ExprNode> lhs)
    : ExprNode(), op(op_kind), lhs(std::move(lhs)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "UnaryNode:\n"
        << make_indent(indent + 1) << "op: " << meaning_from_un_op_kind(op) << "\n"
        << make_indent(indent + 1) << "expr:\n" << lhs->pretty_print(indent + 2);

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct VarDeclarationNode : public StmtNode {
  Ptr<Declaration::Var> var;
  Ptr<ExprNode> init;

  VarDeclarationNode(Ptr<Declaration::Var>&& decl, Ptr<ExprNode>&& init)
    : var(std::move(decl)), init(std::move(init)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "Var:\n";
    out << make_indent(indent + 1) << "mutability: " << meaning_from_mutability(var->mut) << "\n";
    out << make_indent(indent + 1) << "type: " << meaning_from_primitive(var->type) << "\n";
    out << make_indent(indent + 1) << "name: " << var->ident.as_str() << "\n";

    if (init != nullptr) {
      ASSERT(var->init.has_value(), "unexpected behavior");

      out << make_indent(indent + 1) << "initializer:\n";
      out << make_indent(indent + 2) << "op: " << meaning_from_assign_op_kind(var->init.value().assignment);
      out << "\n" << init->pretty_print(indent + 2);
    }

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct AssignmentNode : public StmtNode {
  Expr::Identifier ident;
  Tokenizer::AssignOp op;
  Ptr<ExprNode> expr;

  AssignmentNode(Tokenizer::AssignOp op, Expr::Identifier ident, Ptr<ExprNode>&& expr)
    : StmtNode(), op{op}, ident(std::move(ident)), expr(std::move(expr)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "Assignment:\n";
    out << make_indent(indent + 1) << "ident: " << ident.as_str() << "\n";
    out << make_indent(indent + 1) << "op: " << meaning_from_assign_op_kind(op) << "\n";
    out << make_indent(indent + 1) << "expr:\n" << expr->pretty_print(indent + 2);

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnHeaderNode : public StmtNode {
  Ptr<Declaration::FnHeader> header;

  FnHeaderNode(Declaration::FnHeader&& header) 
    : StmtNode(), header(std::make_unique<Declaration::FnHeader>(std::move(header))) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;
    std::string top_level_indent(indent * 2, ' ');
    std::string children_level_indent((indent + 1) * 2, ' ');

    out << make_indent(indent) << "Header:\n";
    out << make_indent(indent + 1) << "sig: " << header->sig.as_str() << "\n";
    out << make_indent(indent + 1) << "ident: " << header->ident.as_str() << "\n";
    out << make_indent(indent + 1) << "params: ";

    if (header->params.empty()) {
      out << "[]\n";
    } else {
      out << "[";
      for (int k = 0; k < header->params.size(); ++k) {
        Declaration::Parameter& param = header->params.at(k);
        out << param.as_str();
        if (k != header->params.size() - 1) {
          out << ", ";
        }
      }
      out << "]\n";
    }

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct BlockNode : public StmtNode {
  std::vector<Ptr<StmtNode>> nodes;

  BlockNode(std::vector<Ptr<StmtNode>>&& nodes)
    : StmtNode(), nodes(std::move(nodes)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    for (auto& stmt : nodes) {
      if(stmt) {
        out << stmt->pretty_print(indent);
      }
    }

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnNode : public StmtNode {
  Ptr<FnHeaderNode> header;
  Ptr<BlockNode> body;

  FnNode(Ptr<FnHeaderNode>&& header, Ptr<BlockNode>&& body)
    : StmtNode(), header(std::move(header)), body(std::move(body)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "Fn:\n";
    out << make_indent(indent + 1) << "Left:\n";
    out << header->pretty_print(indent + 2);
    out << make_indent(indent + 1) << "Right:\n";
    out << body->pretty_print(indent + 2);

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct ReturnNode : public StmtNode {
  Ptr<ExprNode> expr;

  explicit ReturnNode(Ptr<ExprNode>&& expr)
    : StmtNode(), expr(std::move(expr)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "Return:\n";
    out << make_indent(indent + 1) << "expr:\n" << expr->pretty_print(indent + 2);

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnCallNode : public ExprNode, public StmtNode {
  Identifier ident;
  std::vector<Ptr<ExprNode>> args;

  FnCallNode(Identifier&& ident, std::vector<Ptr<ExprNode>>&& args)
    : ExprNode(), StmtNode(), ident(std::move(ident)), args(std::move(args)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "FnCall:\n";
    out << make_indent(indent + 1) << "ident: " << ident.as_str() << "\n";
    out << make_indent(indent + 1) << "args: ";

    if (args.empty()) {
      out << "[]\n";
    } else {
      out << "[\n";
      for (int k = 0; k < args.size(); ++k) {
        out << args.at(k)->pretty_print(indent + 2);
        if (k != args.size() - 1) {
          out << ", ";
        }
      }
      out << make_indent(indent + 1) << "]\n";
    }

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct ImportNode : public StmtNode {
  Expr::Identifier ident;

  ImportNode(Expr::Identifier&& ident)
    : StmtNode(), ident(std::move(ident)) {}

  std::string pretty_print(int indent = 0) override {
    std::stringstream out;

    out << make_indent(indent) << "Import:\n";
    out << make_indent(indent + 1) << "ident: " << ident.as_str() << "\n";

    return out.str();
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

#endif // NODE_HPP_