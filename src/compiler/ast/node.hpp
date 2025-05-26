#ifndef NODE_HPP_
#define NODE_HPP_

#include <iostream>
#include <optional>
#include <sstream>

#include "expr.hpp"
#include "stmt.hpp"
#include "typing.hpp"
#include "pp_visitor.hpp"
#include "sema_visitor.hpp"

using Declaration::VarInfo;
using Declaration::Parameter;

enum class NodeId : int {
    // Represents a literal value, such as an integer, char, or string.
    Lit,
    // Represents a terminal symbol, such as an identifier or constant reference.
    Term,
    // Represents array subscripting (indexing).
    ArrSub,
    // Represents a binary operation.
    Bin,
    // Represents a unary operation.
    Un,
    // Represents a variable declaration statement.
    VarDecl,
    // Represents an assignment statement.
    //
    // E.g: `x = 7;`
    Assign,
    // Represents a dereference assignment.
    DerefAssign,
    // Represents a function declaration.
    FnDecl,
    // Represents a block of statements.
    Block,
    // Represents a break statement used to exit loops.
    Break,
    // Represents a loop construct.
    Loop,
    // Represents an if-statement or conditional branch.
    If,
    // Represents a return statement.
    Return,
    // Represents an import statement.
    Import,
    // Represents a function call expression.
    FnCall
};

struct Node {
  NodeId id;

  Node(NodeId&& id) : id{std::move(id)} {}

  virtual ~Node() = default;
  virtual void accept(PPVisitor& visitor) = 0;
  virtual void analyze(SemanticVisitor& analyzer) = 0;

  std::string id_str() const noexcept {
    switch(id) {
      case NodeId::Lit:
        return "literal";
      case NodeId::Term:
          return "terminal";
      case NodeId::ArrSub:
          return "array_subscription";
      case NodeId::Bin:
          return "binary_operation";
      case NodeId::Un:
          return "unary_operation";
      case NodeId::VarDecl:
          return "variable_declaration";
      case NodeId::Assign:
          return "assignment";
      case NodeId::DerefAssign:
          return "dereference_assignment";
      case NodeId::FnDecl:
          return "function_declaration";
      case NodeId::Block:
          return "block";
      case NodeId::Break:
          return "break";
      case NodeId::Loop:
          return "loop";
      case NodeId::If:
          return "if";
      case NodeId::Return:
          return "return";
      case NodeId::Import:
          return "import";
      case NodeId::FnCall:
          return "function_call";
      default:
          return "unknown";
    }
  }
};

enum class ValueCategory : int {
    // Represents a value that can be assigned to, i.e., an l-value.
    LValue,
    // Represents a value that can be read from, i.e., an r-value.
    RValue
};

struct StmtNode : virtual public Node {
  ~StmtNode() = default;
  StmtNode(NodeId&& id) : Node(std::move(id)) {}
};

struct ExprNode : virtual public Node {
  // A type attached to the expression during sema analysis.
  SharedPtr<Type> sema_type;
  // The value category of the expression, which can be either l-value or r-value.
  ValueCategory category = ValueCategory::RValue;
  // A mutability flag indicating whether the expression can be modified.
  Mutability mut = Mutability::Immutable;

  ExprNode(NodeId&& id) : Node(std::move(id)), sema_type{nullptr} {}
  ExprNode(ExprNode&&) noexcept = default;
  ExprNode& operator=(ExprNode&&) noexcept = default;

  ExprNode(const ExprNode&) = delete;
  ExprNode& operator=(const ExprNode&) = delete;
};

struct LiteralNode : public ExprNode {
  using LitStr = std::string;

  LitStr str;
  LiteralKind kind;
  Location src_loc;

  LiteralNode(const Expr::Literal& expr) 
    : Node(NodeId::Lit), 
      ExprNode(NodeId::Lit), 
      str(std::move(expr.val)), 
      kind(std::move(expr.kind)), 
      src_loc(std::move(expr.loc)) {}

  inline bool match(LiteralKind k) const {
    return kind == k;
  }

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct VarTerminalNode : public ExprNode {
  Identifier ident;

  explicit VarTerminalNode(Identifier&& ident)
    : Node(NodeId::Term), 
      ExprNode(NodeId::Term), 
      ident(std::move(ident)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
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
    : Node(NodeId::Bin), 
      ExprNode(NodeId::Bin), 
      op{std::move(op_kind)}, 
      lhs(std::move(lhs)), 
      rhs(std::move(rhs)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct UnaryOpNode : public ExprNode {
  UnOpKind op;
  Ptr<ExprNode> lhs;

  UnaryOpNode(UnOpKind op_kind, Ptr<ExprNode> lhs)
    : Node(NodeId::Un),
      ExprNode(NodeId::Un), 
      op{std::move(op_kind)}, 
      lhs(std::move(lhs)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct VarDeclarationNode : public StmtNode {
  VarInfo info;
  Ptr<ExprNode> init;
  Option<AssignOp> op;

  VarDeclarationNode(VarInfo&& info, Option<AssignOp> op, Ptr<ExprNode>&& init)
    : Node(NodeId::VarDecl), 
      StmtNode(NodeId::VarDecl), 
      info(std::move(info)), 
      init(std::move(init)), 
      op(std::move(op)) {}

  inline bool initialized() const {
    return init != nullptr;
  }

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct AssignmentNode : public StmtNode {
  Tokenizer::AssignOp op;
  Identifier lvalue;
  Ptr<ExprNode> rvalue;

  AssignmentNode(Tokenizer::AssignOp op, Identifier lvalue, Ptr<ExprNode>&& rvalue)
    : Node(NodeId::Assign), 
      StmtNode(NodeId::Assign), 
      op{std::move(op)}, 
      lvalue(std::move(lvalue)), 
      rvalue(std::move(rvalue)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct DerefAssignmentNode : public StmtNode {
  Tokenizer::AssignOp op;
  Ptr<ExprNode> lvalue;
  Ptr<ExprNode> rvalue;

  DerefAssignmentNode(
    Tokenizer::AssignOp op, 
    Ptr<ExprNode>&& lvalue, 
    Ptr<ExprNode>&& rvalue
  )
    : Node(NodeId::DerefAssign), 
      StmtNode(NodeId::Assign), 
      op{std::move(op)}, 
      lvalue(std::move(lvalue)), 
      rvalue(std::move(rvalue)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

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
    : Node(NodeId::FnDecl),
      StmtNode(NodeId::FnDecl), 
      name(std::move(header.ident)),
      params(std::move(header.params)),
      ret_type(std::move(header.ret_type)) {} 

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct BlockNode : public StmtNode {
  std::vector<Ptr<StmtNode>> children;

  BlockNode(std::vector<Ptr<StmtNode>>&& nodes)
    : Node(NodeId::Block), 
      StmtNode(NodeId::Block), 
      children(std::move(nodes)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct BreakNode : public StmtNode {
  BreakNode() : Node(NodeId::Break), StmtNode(NodeId::Break) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct LoopNode : public StmtNode {
  Ptr<BlockNode> body;

  LoopNode(Ptr<BlockNode>&& loop_body)
    : Node(NodeId::Loop), 
      StmtNode(NodeId::Loop), 
      body(std::move(loop_body)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct IfNode : public StmtNode {
  Ptr<ExprNode> condition;
  Ptr<BlockNode> if_block;
  Ptr<BlockNode> else_block;

  IfNode(Ptr<ExprNode> condition, Ptr<BlockNode>&& if_block, Ptr<BlockNode>&& else_block)
  : Node(NodeId::If), 
    StmtNode(NodeId::If), 
    condition(std::move(condition)), 
    if_block(std::move(if_block)), 
    else_block(std::move(else_block)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnNode : public StmtNode {
  Ptr<FnHeaderNode> header;
  Ptr<BlockNode> body;

  FnNode(Ptr<FnHeaderNode>&& header, Ptr<BlockNode>&& body)
    : Node(NodeId::FnDecl), 
      StmtNode(NodeId::FnDecl), 
      header(std::move(header)), 
      body(std::move(body)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct ReturnNode : public StmtNode {
  Identifier fn;
  Ptr<ExprNode> expr;

  ReturnNode(Identifier&& ident, Ptr<ExprNode>&& expr)
    : Node(NodeId::Return), 
      StmtNode(NodeId::Return), 
      fn(std::move(ident)), 
      expr(std::move(expr)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct FnCallNode : public ExprNode, public StmtNode {
  Identifier ident;
  std::vector<Ptr<ExprNode>> args;

  FnCallNode(Identifier&& ident, std::vector<Ptr<ExprNode>>&& args)
    : Node(NodeId::FnCall), 
      ExprNode(NodeId::FnCall), 
      StmtNode(NodeId::FnCall), 
      ident(std::move(ident)), 
      args(std::move(args)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

struct ImportNode : public StmtNode {
  Identifier ident;

  ImportNode(Identifier&& ident)
    : Node(NodeId::Import), 
      StmtNode(NodeId::FnCall), 
      ident(std::move(ident)) {}

  void analyze(SemanticVisitor& analyzer) override {
    analyzer.sema_analyze(*this);
  }

  void accept(PPVisitor& visitor) override {
    visitor.visit(*this);
  }
};

#endif // NODE_HPP_