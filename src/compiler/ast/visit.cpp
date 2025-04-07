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

void PPVisitor::visit(LocalDeclNode& ld) {
    std::cout << ld.pretty_print(0);
}