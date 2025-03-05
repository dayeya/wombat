#ifndef EXPR_HPP_
#define EXPR_HPP_

#include <iostream>
#include <optional>
#include <expected>

#include "token.hpp"

enum LogicalOp {
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
    std::optional<LazyTokenStream> tokens;   

    Expr(ExprKind ek)
        : expr_kind(ek), 
          tokens(std::make_optional<LazyTokenStream>()) {} 

    void expand_with(unique_ptr<Token> t) {
        if(!tokens) {
            std::cout << "[Expr::expand_with] could not expand with nullopt";
            return;
        }
        tokens->advance_with_token(std::move(t));
    }
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
struct LitExpr : Expr {
    std::string val_as_str;

    LitExpr(unique_ptr<Token> tok) : Expr(ExprKind::Lit), val_as_str("") {
        expand_with(std::move(tok));
    }
};

#endif // EXPR_HPP_