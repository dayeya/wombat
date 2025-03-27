#ifndef NODE_VISITOR_HPP_
#define NODE_VISITOR_HPP_

#include <wtypes.hpp>

class ValueNode;
class BinOpNode;
class UnaryOpNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ValueNode& vn) = 0;
    virtual void visit(BinOpNode& bn) = 0;
    virtual void visit(UnaryOpNode& bn) = 0;
};

class PrettyPrintVisitor : public Visitor {
public:
    void visit(ValueNode& vn) override;
    void visit(BinOpNode& bn) override;
    void visit(UnaryOpNode& bn) override;
};

#endif // NODE_VISITOR_HPP_
