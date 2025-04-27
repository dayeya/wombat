#include <iostream>

#include "visit.hpp"
#include "ast.hpp"

void PPVisitor::visit(LiteralNode& vn) {
    std::cout << vn.pretty_print(0);
}

void PPVisitor::visit(BinOpNode& bn) {
    std::cout << bn.pretty_print(0);
}

void PPVisitor::visit(UnaryOpNode& on) {
    std::cout << on.pretty_print(0);
}

void PPVisitor::visit(VarDeclarationNode& ld) {
    std::cout << ld.pretty_print(0);
}

void PPVisitor::visit(FnHeaderNode& fh) {
    std::cout << fh.pretty_print(0);
}

void PPVisitor::visit(FnNode& fn) {
    std::cout << fn.pretty_print(0);
}

void PPVisitor::visit(BlockNode& fb) {
    std::cout << fb.pretty_print(0);
}

void PPVisitor::visit(ReturnNode& rn) {
    std::cout << rn.pretty_print(0);
}

void PPVisitor::visit(FnCallNode& fn) {
    std::cout << fn.pretty_print(0);
}

void PPVisitor::visit(AssignmentNode& an) {
    std::cout << an.pretty_print(0);
}

void PPVisitor::visit(ImportNode& im) {
    std::cout << im.pretty_print(0);
}

void PPVisitor::visit(VarTerminalNode& vn) {
    std::cout << vn.pretty_print(0);
}