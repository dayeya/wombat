#ifndef EXPR_HPP_
#define EXPR_HPP_

#include <iostream>
#include <optional>
#include <expected>

#include "token.hpp"

enum class LogicalOp {
    And,
    Or, 
    Not
};

enum class BinOp {
    Plus, 
    Minus, 
    Multi,
    Division,
    Mod
};

enum class UnaryOp {
    Inc, 
    Dec,
    LogicalNot
};

enum class ExprKind {
    Lit,
    BinExpr,
    UnExpr
};


//! Context free grammar part of wombats `cfg`:
//! 
//! lit ::= number
//!       | float
//!       | char
//!       | string -> could be boolean literal
//!
//! bin_op ::= '+' 
//!          | '-' 
//!          | '*' 
//!          | '/' 
//!          | '%'
//!
//! expr ::= lit
//!        | "(" expr ")"
//!        | expr + [bin_op | logical_op] + expr
//! 
//! Examples:
//! 
//! e1 = 1 + 2;
//! e2 = (1 + 2) + 5;
//!
struct Expr {
    ExprKind expr_kind;

    Expr(ExprKind ek) : expr_kind(ek) {}
};

//! @brief Wraps the value of Literal tokens with extended expr parsing api.
//! 
//! Examples:
//!
//!              BinExpr
//!              ~~~~~
//! let c: int = 4 + 4;
//!              ^   ^
//!              LitExpr
//!
struct LitExpr : public Expr {
    SmartPtr<Token> tok;

    LitExpr(SmartPtr<Token>& t)
        : Expr(ExprKind::Lit), tok(std::move(t)) {}
};

#endif // EXPR_HPP_