#ifndef NODE_VISITOR_HPP_
#define NODE_VISITOR_HPP_

class LiteralNode;
class BinOpNode;
class UnaryOpNode;
class LocalDeclNode;

struct PPVisitor {
    void visit(LiteralNode& vn);
    void visit(BinOpNode& bn);
    void visit(UnaryOpNode& un);
    void visit(LocalDeclNode& ld);
};

#endif // NODE_VISITOR_HPP_
