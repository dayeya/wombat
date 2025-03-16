#include "parser.hpp"

auto Parser::parse_primary() -> Ptr<BaseExpr> {
    auto cur = cur_tok();

    if(cur.matches_any(
        TokenKind::LiteralNum, 
        TokenKind::LiteralChar, 
        TokenKind::LiteralString, 
        TokenKind::LiteralBoolean)
    ) {
        // Eat the literal.
        eat();
        return mk_ptr<Expr::Value>(Expr::Value(cur));
    }
}

auto Parser::parse_expr() -> Ptr<BaseExpr> {
    auto base = parse_primary();

    while(
        tok_cur.can_advance() &&
        is_bin_op(cur_tok())
    ) {
        auto op = specify_bin_op(cur_tok().kind);

        // Eat the operator.
        eat();

        auto rhs = parse_primary();

        base = mk_ptr<Expr::BinExpr>(
            Expr::BinExpr(op, std::move(base), std::move(rhs))
        );
    }

    return base;
}

auto Parser::convert_expr_to_ast_node(Ptr<BaseExpr>& expr_ref) -> Ptr<AstNode> {
    if(!expr_ref) {
        WOMBAT_ASSERT(false, "UNEXPECTED NULLPTR");
    }

    switch(expr_ref->kind) {
        case ExprKind::Lit: {
            auto* val_expr = dynamic_cast<Expr::Value*>(expr_ref.get()); 
            return mk_ptr<ValueNode>(ValueNode(*val_expr));
        }
        case ExprKind::Binary: {
            auto* bin_expr = dynamic_cast<Expr::BinExpr*>(expr_ref.get());
            auto lhs = convert_expr_to_ast_node(bin_expr->lhs);
            auto rhs = convert_expr_to_ast_node(bin_expr->rhs);
            return mk_ptr<BinOpNode>(
                BinOpNode(bin_expr->op, std::move(lhs), std::move(rhs))
            );
        }
        default: {
            WOMBAT_ASSERT(false, "UNEXPECTED EXPR_KIND");
            return nullptr;
        }
    }
}

auto Parser::parse() -> AST {
    auto expr = parse_expr();
    auto n = convert_expr_to_ast_node(expr);
    return AST(std::move(n)); 
}