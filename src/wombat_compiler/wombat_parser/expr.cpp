#include "expr.hpp"

using Expr::Precedence;
using Expr::Associativity;

Option<Precedence> Expr::prec_from_bin_op(const BinOpKind& bin_op) {
    switch (bin_op) {
        case BinOpKind::Add:        return Precedence::Sum;
        case BinOpKind::Sub:        return Precedence::Sum;
        case BinOpKind::Mul:        return Precedence::Product;
        case BinOpKind::Div:        return Precedence::Product;
        case BinOpKind::FlooredDiv: return Precedence::Product;
        case BinOpKind::Mod:        return Precedence::Product;
        case BinOpKind::Pow:        return Precedence::Product;
        case BinOpKind::BitAnd:     return Precedence::BitAnd;
        case BinOpKind::BitOr:      return Precedence::BitOr;
        case BinOpKind::BitXor:     return Precedence::BitXor;
        case BinOpKind::Shl:        return Precedence::Shift;
        case BinOpKind::Shr:        return Precedence::Shift;
        case BinOpKind::Lt:         return Precedence::Compare;
        case BinOpKind::Gt:         return Precedence::Compare;
        case BinOpKind::Le:         return Precedence::Compare;
        case BinOpKind::Ge:         return Precedence::Compare;
        case BinOpKind::Eq:         return Precedence::Compare;
        case BinOpKind::NotEq:      return Precedence::Compare;
        case BinOpKind::And:        return Precedence::LogicalAnd;
        case BinOpKind::Or:         return Precedence::LogicalOr;
        default:
            return std::nullopt;
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