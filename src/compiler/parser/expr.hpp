#ifndef EXPR_HPP_
#define EXPR_HPP_

#include <iostream>
#include <optional>
#include <expected>

#include "token.hpp"

using Tokenizer::Identifier;

/*
-- Wombat BNF for expression rules:

Expression := Literal
            | Variable
            | UnaryExpr
            | BinaryExpr
            | Grouping
            | FnCallExpr

Literal := int 
         | float
         | char
         | string 
         | bool

Variable := Identifier

UnaryExpr := UnaryOp Expression
UnaryOp := -
         | not 
         | ~
         | ptr

BinaryExpr := Expression BinaryOp Expression
BinaryOp := + 
          | - 
          | *
          | **
          | / 
          | //
          | % 
          | == 
          | != 
          | < 
          | <= 
          | > 
          | >=
          | and 
          | or

Grouping := ( Expression )

FnCallExpr := <Function_Idenfier>(ArgumentList?)
ArgumentList := Expression (, Expression)*

*/

using Tokenizer::LiteralKind;
using Tokenizer::BinOpKind;
using Tokenizer::UnOpKind;
using Tokenizer::AssignOp;

using Tokenizer::Token;
using Tokenizer::Location;
using Tokenizer::literal_kind_from_token;

namespace Expr {

enum class ExprKind: int {
    // A value expression maps to Expr::Literal.
    // E.g `1, true, foo, 1.0, "bar"`
    Lit,
    // A binary expression maps to Expr::BinExpr.
    Binary,
    // An unary expression, operator and the inner expression.
    // E.g '-1', 'not true', '~foo'
    Unary,
    // An expression within parenthesis.
    // E.g '(1 + 2)'
    Group,
    // A function call expression. Expr::FnCall.
    // E.g 'foo(1, 2, 3)'
    FnCall,
    // A named variable.
    // E.g 'foo'
    Local,
    // An array subscription.
    // E.g 'foo[0]'
    ArraySubscription,
};

struct BaseExpr {
    ExprKind kind;

    BaseExpr(ExprKind k) : kind(k) {}
    virtual ~BaseExpr() = default;
    
    inline bool matches(ExprKind kind) const { 
        return kind == kind; 
    } 
};

struct Literal : public BaseExpr {
    std::string val;
    LiteralKind kind;
    Location loc = Location::Singularity();

    explicit Literal() : BaseExpr(ExprKind::Lit) {}

    explicit Literal(const Token& t): BaseExpr(ExprKind::Lit) {
        val = t.value;
        loc = t.loc;
        kind = literal_kind_from_token(t.kind).value_or(LiteralKind::None);
    }
};

struct Local : public BaseExpr {
    Identifier ident;

    Local(Identifier&& ident) 
        : BaseExpr(ExprKind::Local), ident(std::move(ident)) {};
};

struct FnCall : public BaseExpr {
    Identifier ident;
    std::vector<Ptr<BaseExpr>> args;

    FnCall(Identifier&& ident, std::vector<Ptr<BaseExpr>>&& args) 
        : BaseExpr(ExprKind::FnCall), ident(std::move(ident)), args(std::move(args)) {}
};

struct BinExpr : public BaseExpr {
    BinOpKind op;
    Ptr<BaseExpr> lhs;
    Ptr<BaseExpr> rhs;

    BinExpr() : BaseExpr(ExprKind::Binary) {}

    BinExpr(
        BinOpKind op, 
        Ptr<BaseExpr> lhs, 
        Ptr<BaseExpr> rhs
    ) : BaseExpr(ExprKind::Binary), op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

struct GroupExpr : public BaseExpr {
    Ptr<BaseExpr> expr;
    
    GroupExpr(Ptr<BaseExpr>& e) : BaseExpr(ExprKind::Group), expr(std::move(e)) {} 
};

struct UnaryExpr : public BaseExpr {
    UnOpKind op;
    Ptr<BaseExpr> expr;

    UnaryExpr(UnOpKind op, Ptr<BaseExpr>& e) : BaseExpr(ExprKind::Unary), op(op), expr(std::move(e)) {}
};

struct ArraySubscription : public BaseExpr {
    Identifier arr;
    Ptr<BaseExpr> index;

    ArraySubscription(Identifier&& arr, Ptr<BaseExpr>&& index) 
        : BaseExpr(ExprKind::ArraySubscription), arr(std::move(arr)), index(std::move(index)) {}
};

enum class Precedence : int {
    Dummy       = 0,  // Lowest precedence (used for undefined cases)
    LogicalOr   = 1,  // `or`
    LogicalAnd  = 2,  // `and`
    Compare     = 3,  // ==, !=, <, >, <=, >=
    BitOr       = 4,  // |
    BitXor      = 5,  // ^
    BitAnd      = 6,  // &
    Shift       = 7,  // <<, >>
    Sum         = 8,  // +, -
    Product     = 9,  // *, /, %, //
    Pow         = 10, // **
    Prefix      = 11, // Unary -, `not`
    Unambiguous = 12  // Function calls (highest precedence)
};


enum class Associativity: int {
    // operator is associative from the right
    Right,
    // operator is associative from the left
    Left,
    // operator is not associative
    None
};

// Returns the corresponding precedence of a binary operator.
Precedence prec_from_bin_op(const BinOpKind& bin_op);

// Returns the corresponding precedence of an unary operator.
Precedence prec_for_un_op(const UnOpKind& un_op);

// Returns the corresponding associativity for a binary operator.
Associativity assoc_from_bin_op(const BinOpKind& bin_op);

// Returns the corresponding associativity for an unary operator.
Associativity assoc_from_un_op();

std::string meaning_from_expr_kind(const ExprKind& kind);

};

#endif // EXPR_HPP_