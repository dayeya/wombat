#ifndef WALK_HPP_
#define WALK_HPP_

#include "ast.hpp"
#include "stmt.hpp"

using Declaration::Primitive;
using Declaration::FnSignature;

struct TypeWalker {
    void walk_literal_expr(LiteralNode& node, Primitive& expected_type);
    void walk_binary_expr(BinOpNode& node, Primitive& expected_type);
    void walk_fn_call_expr(FnCallNode& node, Declaration::FnSignature& expected_signature);
    void walk_import_stmt(ImportNode& node);
};

#endif // WALK_HPP_