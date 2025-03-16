#include "visit.hpp"
#include "ast.hpp"  // Now it's safe to include because we're in a .cpp file

#include <iostream>

void PrettyPrintVisitor::visit(ValueNode& vn) {
    std::cout << vn.pretty_print(0) << std::endl;
}

void PrettyPrintVisitor::visit(BinOpNode& bn) {
    std::cout << bn.pretty_print(0) << std::endl;
}
