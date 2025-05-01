#include <iostream>

#include <format>
#include "token.hpp"
#include "ast.hpp"

using Tokenizer::meaning_from_kind;
using Tokenizer::meaning_from_assign_op_kind;
using Tokenizer::meaning_from_bin_op_kind;
using Tokenizer::meaning_from_literal_kind;
using Tokenizer::meaning_from_un_op_kind;

void PPVisitor::visit(LiteralNode& ln) {
    ASSERT(!ln.src_loc.eq(Location::Singularity()), "cannot pretty-print a literal without a value.");

    print_node_header("LiteralNode");
    print(format("Kind: {}\n", meaning_from_literal_kind(ln.kind)));
    print(format("Value: {}\n", ln.str));
    decrease_depth();
}

void PPVisitor::visit(BinOpNode& bn) {
    print_node_header("BinaryNode");
    print(format("Op: {}\n", meaning_from_bin_op_kind(bn.op)));

    print_node_header("Left");
    bn.lhs->accept(*this);
    decrease_depth();

    print_node_header("Right");
    bn.rhs->accept(*this);
    decrease_depth();
}

void PPVisitor::visit(UnaryOpNode& un) {
    print_node_header("UnaryNode");
    print(format("Op: {}\n", meaning_from_un_op_kind(un.op)));

    print_node_header("Left");
    un.lhs->accept(*this);
    decrease_depth();
}

void PPVisitor::visit(VarTerminalNode& vn) {
    print_node_header("VarTerminal");
    print(format("Name: {}\n", vn.ident.as_str()));
    decrease_depth();
}

void PPVisitor::visit(ArraySubscriptionNode& an) {
    print_node_header("ArraySubscription");
    print(format("Name: {}\n ", an.arr.as_str()));
    print_node_header("Index");
    an.index->accept(*this);
    decrease_depth(2);
}

void PPVisitor::visit(VarDeclarationNode& vdn) {
    print_node_header("Var");
    print(format("Mut: {}\n", meaning_from_mutability(vdn.info.mut)));
    print(format("Type: {}\n", vdn.info.type->as_str()));
    print(format("Name: {}\n", vdn.info.ident.as_str()));

    if (vdn.init != nullptr && vdn.op.has_value()) {
        print_node_header("Initializer");
        print(format("Op: {}\n", meaning_from_assign_op_kind(vdn.op.value())));
        vdn.init->accept(*this);
        decrease_depth();
    }
}

void PPVisitor::visit(FnHeaderNode& fh) {
    print_node_header("Header");
    print("Sig: {}\n");
    print("Name: {}\n");

    print("Params: ");
    if(fh.params.empty()) 
    {
        print_raw("{}\n");
    } 
    else 
    {   
        int cur = 0;
        int size = fh.params.size();
        print_raw("{\n");
        increase_depth();
        while(cur < size) {
            auto& param = fh.params.at(cur);
            print(param.as_str());
            if(cur != size - 1) {
                print_raw(",\n");
            }
            cur++;
        }
        print_raw("\n");
        decrease_depth();
        print("}\n");
    }
    decrease_depth();
}

void PPVisitor::visit(FnNode& fn) {
    print_node_header("Fn");

    print_node_header("Left");
    fn.header->accept(*this);
    decrease_depth();

    print_node_header("Right");
    fn.body->accept(*this);
    decrease_depth();
}

void PPVisitor::visit(BlockNode& fb) {
    print_node_header("Block");
    for (auto& child : fb.children) {
        if(child) {
            child->accept(*this);
            decrease_depth();
        }
    }
}

void PPVisitor::visit(FnCallNode& fn) {
    print_node_header("FnCall");
    print(format("Name: {}\n", fn.ident.as_str()));

    print("Args: ");
    if (fn.args.empty()) 
    {
      print_raw("{}\n");
    } 
    else 
    {
        int cur = 0;
        int size = fn.args.size();
        print_raw("{\n");
        increase_depth();
        while(cur < size) {
            auto& arg = fn.args.at(cur);

            int cur_depth = depth;
            arg->accept(*this);
            int new_depth = depth;

            while(cur_depth < new_depth--) {
                decrease_depth();
            }

            cur++;
        }
        print_raw("\n");
        decrease_depth();
        print("}\n");
    }
    decrease_depth();
}

void PPVisitor::visit(AssignmentNode& an) {
    print_node_header("Assignment");
    print(format("ident: {}\n", an.ident.as_str()));
    print(format("op: {}\n", meaning_from_assign_op_kind(an.op)));

    print_node_header("Expr");
    an.expr->accept(*this);
    decrease_depth(2);
}

void PPVisitor::visit(ReturnNode& rn) {
    print_node_header("Return");
    print_node_header("Expr:");
    rn.expr->accept(*this);
    decrease_depth();
}

void PPVisitor::visit(ImportNode& im) {
    print_node_header("Import");
    print(format("Module: {}\n", im.ident.as_str()));
    decrease_depth();
}