#include "parser.hpp"

using Tokenizer::Token;
using Tokenizer::TokenKind;

using Expr::ExprKind;

void Parser::eat_and_expect(Closure<bool, Token&> condition, std::string expect) {
    eat();
    if(condition(cur_tok())) return;
    WOMBAT_ASSERT(false, expect);
}

auto Parser::parse_expr(Expr::Precedence min_prec) -> Ptr<Expr::BaseExpr> {
    auto base = parse_primary();

    auto bin_op_match = Tokenizer::bin_op_from_token(cur_tok());
    while(
        bin_op_match.has_value() && 
        Expr::prec_from_bin_op(bin_op_match.value()) >= min_prec
    ) {
        auto op = bin_op_match.value();

        // Eat the operator.
        eat();

        auto prec = [&]() -> Expr::Precedence {
            auto assoc = Expr::assoc_from_bin_op(op);
            auto base_prec = Expr::prec_from_bin_op(op);
        
            if (assoc == Expr::Associativity::Right) 
                return base_prec.value();
        
            if (assoc == Expr::Associativity::Left) {
                using U = std::underlying_type_t<Expr::Precedence>;
                U next_prec = static_cast<U>(base_prec.value()) + 1;
        
                if (next_prec <= static_cast<U>(Expr::Precedence::Unambiguous))
                    return static_cast<Expr::Precedence>(next_prec);
                
                return base_prec.value(); // Prevent overflow
            }
        
            return Expr::Precedence::Dummy;
        }();
        
        auto rhs = parse_expr(prec);

        base = mk_ptr<Expr::BinExpr>(
            Expr::BinExpr(op, std::move(base), std::move(rhs))
        );

        // Continue the matching
        bin_op_match = Tokenizer::bin_op_from_token(cur_tok());
    }

    return base;
}

auto Parser::parse_primary() -> Ptr<Expr::BaseExpr> {
    auto cur = cur_tok();

    if(
        auto un_op_match = Tokenizer::un_op_from_token(cur);
        un_op_match.has_value()
    ) {
        auto prec = Expr::prec_for_un_op(un_op_match.value());
        // Eat the operator.
        eat();
        auto expr = parse_expr(prec);
        return mk_ptr<Expr::UnaryExpr>(Expr::UnaryExpr(un_op_match.value(), expr));
    } else if(cur.match_kind(TokenKind::OpenParen)) {
        // Eat the open paren.
        eat();
        auto expr = parse_expr(Expr::Precedence::Dummy);
        if(cur_tok().match_kind(TokenKind::CloseParen)) {
            // Eat the open paren.
            eat();
            return mk_ptr<Expr::ParenExpr>(Expr::ParenExpr(expr));
        } else {
            WOMBAT_ASSERT(false, "EXPECTED CLOSING PAREN");
        }
    } else if(cur.matches_any(
        TokenKind::LiteralNum, 
        TokenKind::LiteralFloat, 
        TokenKind::LiteralChar, 
        TokenKind::LiteralString, 
        TokenKind::LiteralBoolean)
    ) {
        // Eat the literal.
        eat();
        return mk_ptr<Expr::Value>(Expr::Value(cur));
    } else {
        std::string err("UNEXPECTED TOKEN: ");
        err.append(cur.value);
        WOMBAT_ASSERT(false, err);
    }

    return nullptr;
}

auto Parser::convert_expr_to_ast_node(Ptr<Expr::BaseExpr>& expr_ref) -> Ptr<AstNode> {
    if(!expr_ref) {
        WOMBAT_ASSERT(false, "UNEXPECTED NULLPTR");
    }

    switch(expr_ref->kind) {
        case ExprKind::Lit: {
            auto* val_expr = dynamic_cast<RawPtr<Expr::Value>>(expr_ref.get()); 
            return mk_ptr<ValueNode>(ValueNode(*val_expr));
        }
        case ExprKind::Binary: {
            auto* bin_expr = dynamic_cast<RawPtr<Expr::BinExpr>>(expr_ref.get());
            auto lhs = convert_expr_to_ast_node(bin_expr->lhs);
            auto rhs = convert_expr_to_ast_node(bin_expr->rhs);
            return mk_ptr<BinOpNode>(
                BinOpNode(bin_expr->op, std::move(lhs), std::move(rhs))
            );
        }
        case ExprKind::Unary: {
            auto* unary = dynamic_cast<RawPtr<Expr::UnaryExpr>>(expr_ref.get());
            auto expr = convert_expr_to_ast_node(unary->expr);
            return mk_ptr<UnaryOpNode>(UnaryOpNode(unary->op, std::move(expr)));
        }
        case ExprKind::Paren: {
            auto* paren_expr = dynamic_cast<RawPtr<Expr::ParenExpr>>(expr_ref.get()); 
            return convert_expr_to_ast_node(paren_expr->expr);
        }
        default: {
            WOMBAT_ASSERT(false, "UNEXPECTED EXPR_KIND");
            return nullptr;
        }
    }
}

auto Parser::parse() -> AST {
    auto expr = parse_expr(Expr::Precedence::Dummy);
    auto n = convert_expr_to_ast_node(expr);
    return AST(std::move(n)); 
}