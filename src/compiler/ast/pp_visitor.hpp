#ifndef VISITOR_HPP_
#define VISITOR_HPP_

#include <format>
using std::format;

struct LiteralNode;
struct BinOpNode;
struct UnaryOpNode;
struct VarDeclarationNode;
struct VarTerminalNode;
struct AssignmentNode;
struct FnHeaderNode;
struct FnNode;
struct BlockNode;
struct LoopNode;
struct IfNode;
struct BreakNode;
struct ReturnNode;
struct FnCallNode;
struct ImportNode;

struct PPVisitor {
    std::ostream& buffer;
    int depth = 0;

    // A single indentation is 2 spaces.
    static CONST int AST_INDENT_WIDTH = 2;

    PPVisitor(std::ostream& dst) 
        : buffer(dst) {}

    void print_raw(const std::string& str) {
        buffer << str;
    }
    
    void print(const std::string& stringified_node) {
        buffer << std::string(depth * AST_INDENT_WIDTH, ' ') << stringified_node;
    }

    // An helper function to print the header.
    // N.B. Every header will contains children so we increase the depth.
    void print_node_header(const std::string& node_name) {
        print(format("{}:\n", node_name));
        increase_depth();
    }

    void increase_depth(int factor = 1) { 
        depth += factor; 
    }

    void decrease_depth(int factor = 1) { 
        depth -= factor; 
    }

    void visit(LiteralNode& vn);
    void visit(BinOpNode& bn);
    void visit(UnaryOpNode& un);
    void visit(VarDeclarationNode& ln);
    void visit(FnHeaderNode& fh);
    void visit(FnNode& fn);
    void visit(BlockNode& bn);
    void visit(LoopNode& ln);
    void visit(IfNode& cfn);
    void visit(ReturnNode& rn);
    void visit(FnCallNode& fn);
    void visit(AssignmentNode& an);
    void visit(ImportNode& im);
    void visit(BreakNode& brk);
    void visit(VarTerminalNode& vn);
};

#endif // VISITOR_HPP_
