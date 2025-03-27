#include "expr.hpp"

using Expr::Precedence;
using Expr::Associativity;

Option<Precedence> Expr::prec_from_bin_op(const BinOpKind& bin_op) {
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