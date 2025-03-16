#ifndef NODE_VISITOR_HPP_
#define NODE_VISITOR_HPP_

class ValueNode;
class BinOpNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ValueNode& vn) = 0;
    virtual void visit(BinOpNode& bn) = 0;
};

class PrettyPrintVisitor : public Visitor {
public:
    void visit(ValueNode& vn) override;
    void visit(BinOpNode& bn) override;
};

#endif // NODE_VISITOR_HPP_
