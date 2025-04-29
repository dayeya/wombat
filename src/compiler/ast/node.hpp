#ifndef NODE_HPP_
#define NODE_HPP_

#include <iostream>
#include <optional>
#include <sstream>

#include "expr.hpp"
#include "stmt.hpp"
#include "pp_visitor.hpp"

using Declaration::Type;
using Declaration::TypeHash;
using Declaration::Parameter;
using Declaration::VarInfo;

struct Node {
  virtual ~Node() = default;
  virtual void accept(PPVisitor& visitor) = 0;
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

  LiteralNode() : ExprNode(), val(std::nullopt) {}
  LiteralNode(const Expr::Literal& val) : ExprNode(), val(val) {}

  inline bool empty() const { return !val.has_value(); }

  inline bool match(LiteralKind kind) const {
    return !empty() && val->kind == kind;
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct BinOpNode : public ExprNode {
  BinOpKind op;
  Ptr<ExprNode> lhs;
  Ptr<ExprNode> rhs;

  BinOpNode(BinOpKind op_kind, Ptr<ExprNode> lhs, Ptr<ExprNode> rhs)
    : ExprNode(), op(op_kind), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct VarTerminalNode : public ExprNode {
  Expr::Identifier ident;

  explicit VarTerminalNode(Expr::Identifier&& ident)
    : ExprNode(), ident(std::move(ident)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct ArraySubscriptionNode : public ExprNode {
  Expr::Identifier arr;
  Ptr<ExprNode> index;

  ArraySubscriptionNode(Expr::Identifier&& arr, Ptr<ExprNode>&& index)
    : ExprNode(), arr(std::move(arr)), index(std::move(index)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct UnaryOpNode : public ExprNode {
  UnOpKind op;
  Ptr<ExprNode> lhs;

  explicit UnaryOpNode(UnOpKind op_kind, Ptr<ExprNode> lhs)
    : ExprNode(), op(op_kind), lhs(std::move(lhs)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct VarDeclarationNode : public StmtNode {
  VarInfo info;
  Ptr<ExprNode> init;
  Option<AssignOp> op;

  VarDeclarationNode(VarInfo&& info, Option<AssignOp> op, Ptr<ExprNode>&& init)
    : info(std::move(info)), init(std::move(init)), op(std::move(op)) {}

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

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnHeaderNode : public StmtNode {
  using FnParams = std::vector<Declaration::Parameter>;

  Identifier name;
  FnParams params;
  SharedPtr<Type> ret_type;

  FnHeaderNode(Declaration::FnHeader&& header) 
    : StmtNode(), 
      name(std::move(header.ident)),
      params(std::move(header.params)),
      ret_type(std::move(header.ret_type)) {} 

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct BlockNode : public StmtNode {
  std::vector<Ptr<StmtNode>> children;

  BlockNode(std::vector<Ptr<StmtNode>>&& nodes)
    : StmtNode(), children(std::move(nodes)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnNode : public StmtNode {
  Ptr<FnHeaderNode> header;
  Ptr<BlockNode> body;

  FnNode(Ptr<FnHeaderNode>&& header, Ptr<BlockNode>&& body)
    : StmtNode(), header(std::move(header)), body(std::move(body)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct ReturnNode : public StmtNode {
  Ptr<ExprNode> expr;

  explicit ReturnNode(Ptr<ExprNode>&& expr)
    : StmtNode(), expr(std::move(expr)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnCallNode : public ExprNode, public StmtNode {
  Identifier ident;
  std::vector<Ptr<ExprNode>> args;

  FnCallNode(Identifier&& ident, std::vector<Ptr<ExprNode>>&& args)
    : ExprNode(), StmtNode(), ident(std::move(ident)), args(std::move(args)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct ImportNode : public StmtNode {
  Expr::Identifier ident;

  ImportNode(Expr::Identifier&& ident)
    : StmtNode(), ident(std::move(ident)) {}

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

#endif // NODE_HPP_