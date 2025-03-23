#include <iostream>

#include "visit.hpp"
#include "ast.hpp"

void PrettyPrintVisitor::visit(ValueNode& vn) {
    std::cout << vn.pretty_print(0);
}

void PrettyPrintVisitor::visit(BinOpNode& bn) {
    std::cout << bn.pretty_print(0);
}

void PrettyPrintVisitor::visit(UnaryOpNode& on) {
    std::cout << on.pretty_print(0);
}

