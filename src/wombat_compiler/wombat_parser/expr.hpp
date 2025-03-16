#ifndef EXPR_HPP_
#define EXPR_HPP_

#include <iostream>
#include <optional>
#include <expected>
#include <cassert>

#include "common.hpp"
#include "token.hpp"

enum class ExprKind {
    // A value expression maps to Expr::Value.
    Lit,
    // A binary expression maps to Expr::BinExpr.
    Binary,
    // A function call expression. Expr::FnCall
    FnCall
};

LiteralKind specify_literal_kind(const TokenKind& kind);
BinaryOperator specify_bin_op(const TokenKind& kind);

std::string lit_kind_to_str(const LiteralKind& kind);
std::string bin_op_kind_to_str(const BinaryOperator& kind);

// Base struct for expressions.
struct BaseExpr {
    ExprKind kind;

    BaseExpr(ExprKind k) : kind(k) {}
    virtual ~BaseExpr() = default;

    
    inline bool matches(ExprKind ek) const {
        return kind == ek;
    } 
};

struct Expr {
    struct Value : public BaseExpr {
        LiteralKind lit_kind;
        std::string val;
        Location loc = SINGULARITY;

        explicit Value() : BaseExpr(ExprKind::Lit) {}

        explicit Value(const Token& t): BaseExpr(ExprKind::Lit) {
            lit_kind = specify_literal_kind(t.kind);
            val = t.value;
            loc = t.loc;
        }
    };

    struct BinExpr : public BaseExpr {
        BinaryOperator op;
        Ptr<BaseExpr> lhs;
        Ptr<BaseExpr> rhs;

        explicit BinExpr() : BaseExpr(ExprKind::Binary) {}

        explicit BinExpr(
            BinaryOperator op, 
            Ptr<BaseExpr> lhs, 
            Ptr<BaseExpr> rhs
        ) : BaseExpr(ExprKind::Binary), op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    };
};

#endif // EXPR_HPP_