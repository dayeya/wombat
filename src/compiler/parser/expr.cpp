#include "parser.hpp"
#include "common.hpp"

using Expr::ExprKind;
using Expr::Precedence;
using Expr::Associativity;

std::string Expr::meaning_from_expr_kind(const ExprKind& kind) {
    switch(kind) {
        case ExprKind::Lit: return "Literal";
        case ExprKind::Binary: return "Binary";
        case ExprKind::Unary: return "Unary";
        case ExprKind::Group: return "Group";
        case ExprKind::FnCall: return "Function";
        case ExprKind::Local: return "Local_Resource_Access";
        default: {
            return "unknown_Expr_Kind";
        }
    }
}

Precedence Expr::prec_from_bin_op(const BinOpKind& bin_op) {
    switch (bin_op) {
        case BinOpKind::Add:        
        case BinOpKind::Sub:        
            return Precedence::Sum;
        case BinOpKind::Mul:        
        case BinOpKind::Div:       
        case BinOpKind::FlooredDiv:
        case BinOpKind::Mod:
            return Precedence::Product;
        case BinOpKind::Pow:        
            return Precedence::Pow;
        case BinOpKind::BitAnd:     
            return Precedence::BitAnd;
        case BinOpKind::BitOr:      
            return Precedence::BitOr;
        case BinOpKind::BitXor:     
            return Precedence::BitXor;
        case BinOpKind::Shl:
        case BinOpKind::Shr:        
            return Precedence::Shift;
        case BinOpKind::Lt: 
        case BinOpKind::Gt: 
        case BinOpKind::Le: 
        case BinOpKind::Ge: 
        case BinOpKind::Eq: 
        case BinOpKind::NotEq:      
            return Precedence::Compare;
        case BinOpKind::And:        
            return Precedence::LogicalAnd;
        case BinOpKind::Or:         
            return Precedence::LogicalOr;
        default:
            return Precedence::Dummy;
    }
}

Precedence Expr::prec_for_un_op(const UnOpKind& un_op) {
    return Precedence::Prefix;
}

Associativity Expr::assoc_from_bin_op(const BinOpKind& bin_op) {
    if(bin_op == BinOpKind::Pow) {
        return Associativity::Right;
    }
    return Associativity::Left;
}

Associativity Expr::assoc_from_un_op() {
    return Associativity::Right;
}

Expr::Precedence Parser::rhs_expr_precedence(Tokenizer::BinOpKind binary_op) {
    Expr::Associativity assoc = Expr::assoc_from_bin_op(binary_op);
    Expr::Precedence prec = Expr::prec_from_bin_op(binary_op);

    if (assoc == Expr::Associativity::Right) 
        return prec;

    if (assoc == Expr::Associativity::Left) {
        int next_prec = static_cast<int>(prec) + 1;

        if (next_prec <= static_cast<int>(Expr::Precedence::Unambiguous))
            return static_cast<Expr::Precedence>(next_prec);
        
        return prec;
    }

    return Expr::Precedence::Dummy;
}

Ptr<Expr::UnaryExpr> Parser::expr_unary() {
    ASSERT(unary(), "unreachable: cannot parse something that is not an unary.");

    Tokenizer::UnOpKind unary_op = Tokenizer::un_op_from_token(cur_tok()).value();
    Expr::Precedence prec = Expr::prec_for_un_op(unary_op);
    
    // Eat the operator.
    eat();

    auto unary_expr = expr(prec);
    return mk_ptr(Expr::UnaryExpr(unary_op, unary_expr));
}

Ptr<Expr::GroupExpr> Parser::expr_group() {
    ASSERT(group_start(), "unreachable: group does not start with an opening parenthesis.");

    // Eat the open paren.
    eat();
    auto group = expr(Expr::Precedence::Dummy);

    ASSERT(
        group_end(), 
        "invalid use of parenthesis, expected `)` but got `" + cur_tok().value  + "`"
    );

    // Eat the closing paren.
    eat();
    return mk_ptr(Expr::GroupExpr(group));
}

Ptr<Expr::Literal> Parser::expr_literal() {
    Expr::Literal literal(cur_tok());

    // Eat the literal itself.
    eat();
    return mk_ptr(std::move(literal));
}

Ptr<Expr::Local> Parser::expr_ident_local() {
    ASSERT(cur_tok().match_kind(TokenKind::Identifier), "unreachable: expected an identifier.");

    Expr::Identifier ident(cur_tok().value);
    eat();

    return mk_ptr(Expr::Local(std::move(ident)));
}

Ptr<Expr::FnCall> Parser::expr_ident_fn() {
    ASSERT(cur_tok().match_kind(TokenKind::Identifier), "unreachable: expected an identifier.");

    Expr::Identifier ident(cur_tok().value);
    eat();

    // Eat the open paren.
    ASSERT(
        cur_tok().match_kind(TokenKind::OpenParen), 
        std::format("expected `(` but got `{}`", cur_tok().value)
    );
    eat();

    std::vector<Ptr<Expr::BaseExpr>> args;
    while(!cur_tok().match_kind(TokenKind::CloseParen)) {
        auto arg = expr(Expr::Precedence::Dummy);
        args.push_back(std::move(arg));

        if(cur_tok().match_kind(TokenKind::Comma)) {
            eat();
            continue;
        }
        break;
    }

    // Eat the closing paren.
    ASSERT(
        cur_tok().match_kind(TokenKind::CloseParen), 
        std::format("expected `)` but got `{}`", cur_tok().value)
    );
    eat();

    return mk_ptr(Expr::FnCall(std::move(ident), std::move(args)));
}

Ptr<Expr::BaseExpr> Parser::expr_primary() {
    if(unary()) {
        return expr_unary();
    } 
    if(group_start()) {
        return expr_group();
    }
    if(literal()) {
        return expr_literal();
    }
    if(cur_tok().match_kind(TokenKind::Identifier)) {
        // First, check if the 'ident' could be a function call.
        if(
            ntok_for([](Token& tok) {
                return tok.match_kind(TokenKind::OpenParen); 
            }, TokDistance::Next)
        ) {
            return expr_ident_fn();
        }
        return expr_ident_local();
    }
    ASSERT(false, "invalid token: got `" + cur_tok().value + "`, expected expression");
    return nullptr;
}

Ptr<Expr::BaseExpr> Parser::expr(Expr::Precedence min_prec) {
    auto base = expr_primary();

    auto bin_op_match = Tokenizer::bin_op_from_token(cur_tok());
    while(
        bin_op_match.has_value() && 
        Expr::prec_from_bin_op(bin_op_match.value()) >= min_prec
    ) {
        auto op = bin_op_match.value();

        // Eat the operator.
        eat();
        
        auto rhs = expr(rhs_expr_precedence(op));

        base = mk_ptr<Expr::BinExpr>(
            Expr::BinExpr(op, std::move(base), std::move(rhs))
        );

        // Continue the matching
        bin_op_match = bin_op_from_token(cur_tok());
    }

    return base;
}

Ptr<Expr::BaseExpr> Parser::parse_expr_without_recovery() {
    return expr(Expr::Precedence::Dummy);
}