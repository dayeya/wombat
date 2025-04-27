#ifndef NODE_VISITOR_HPP_
#define NODE_VISITOR_HPP_

struct LiteralNode;
struct BinOpNode;
struct UnaryOpNode;
struct VarDeclarationNode;
struct VarTerminalNode;
struct AssignmentNode;
struct FnHeaderNode;
struct FnNode;
struct BlockNode;
struct ReturnNode;
struct FnCallNode;
struct ImportNode;

struct PPVisitor {
    void visit(LiteralNode& vn);
    void visit(BinOpNode& bn);
    void visit(UnaryOpNode& un);
    void visit(VarDeclarationNode& ln);
    void visit(FnHeaderNode& fh);
    void visit(FnNode& fn);
    void visit(BlockNode& bn);
    void visit(ReturnNode& rn);
    void visit(FnCallNode& fn);
    void visit(AssignmentNode& an);
    void visit(ImportNode& im);
    void visit(VarTerminalNode& vn);
};

#endif // NODE_VISITOR_HPP_
