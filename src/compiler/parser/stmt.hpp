#ifndef STMT_HPP_
#define STMT_HPP_

#include <format>
#include <vector>
#include <list>
#include <memory>
#include <optional>

#include "expr.hpp"

/*
-- Wombat BNF for statement rules:

Stmt := SimpleStmt | CompoundStmt
SimpleStmt := Declaration | ReturnStmt | FnCallStmt
CompundStmt := IfStmt | ElseStmt | LoopStmt | MatchStmt
 
For `Declaration` see `[decl.hpp]` for the grammar.

ReturnStmt := return <expression>;

FnCallStmt := <function_identifier>(<Param>, ...);
Param := expression

IfStmt := if <expression> { Scope }
ElseStmt := else { Scope }

BasicLoopStmt := loop { Scope }
LoopWithStmt := loop with <Declaration> { Scope }
LoopStmt := BasicLoopStmt | LoopWith

-- Scope is a collection of one or more statements.
Scope := Stmt*

Pattern := LiteralPattern | IdentifierPattern | WildcardPattern
LiteralPattern := int_literal | string_literal | bool_literal
IdentifierPattern := Identifier
WildcardPattern := _

MatchArm := | [pattern] -> { Scope }
MatchStmt := match <expression> with 
             MatchArm*
             end
*/
namespace Statement {

enum class StmtKind : int {
    // Import a module.
    // E.g 'import foo' or 'import @local.bar'
    Import,
    // A local variable declaration.
    // E.g 'let foo: bool = True;
    Local,
    // A function declaration.
    // E.g 'fn int32 foo(mut bar: int32) ... end'
    FnInvoke
};

struct Stmt {
    StmtKind kind;

    Stmt(StmtKind kind) : kind(kind) {}
    virtual ~Stmt() = default;
};

struct FnCall : private Stmt { 
    using FnParams = std::vector<Expr::BaseExpr>;

    Expr::Identifier fn_ident;

    FnCall() : Stmt(StmtKind::FnInvoke), fn_ident() {}
};

struct Scope {
    using StmtList = std::list<Stmt>;
    
    // A scope is just a list of statements to be executed.
    StmtList stmts;

    Scope() : stmts() {}
};

inline std::string meaning_from_stmt_kind(const StmtKind& kind) {
    switch(kind) {
        case StmtKind::Local: return "stmt_LOCAL";
        case StmtKind::Import: return "stmt_IMPORT"; 
        case StmtKind::FnInvoke: return "stmt_FN_CALL";
        default: {
            UNREACHABLE();
            return "stmt_UNKNOWN"; // Add a return for safety
        }
    }
}
}

namespace Declaration {

using Keyword = Tokenizer::Keyword;
using Stmt = Statement::Stmt;
using StmtKind = Statement::StmtKind;

enum class Primitive : int {
    Free,
    Int, 
    Float,
    Char,
    Boolean,
    Ptr
};

enum class Mutability : int { Immutable, Mutable };

struct Initializer : public Stmt {
    Ptr<Expr::BaseExpr> expr;
    Tokenizer::AssignOp assignment;

    Initializer(const Tokenizer::AssignOp& op, Ptr<Expr::BaseExpr> expr)
        : Stmt(StmtKind::Local), expr(std::move(expr)), assignment(std::move(op)) {}
};

struct Var : public Stmt {
    Mutability mut;
    Primitive type;
    Expr::Identifier ident;
    Option<Initializer> init;

    Var(
        StmtKind kind,
        Mutability mut,
        Primitive type,
        Expr::Identifier ident,
        Option<Initializer> init
    ) : Stmt(kind),
        mut{mut},
        type{type},
        ident{ident},
        init{std::move(init)} {}

    bool is_initialized() const {
        return init.has_value();
    }

    const Ptr<Expr::BaseExpr>& initializer_expr() {
        return init.value().expr;
    }
};

struct Fn : public Stmt {
    using FnScope = Ptr<Statement::Scope>;
    using FnParams = std::vector<Var>;

    Primitive output;    
    Expr::Identifier ident;
    FnParams params;
    FnScope scope;
};

inline std::string meaning_from_mutability(const Mutability& mut) {
    switch(mut) {
        case Mutability::Mutable: return "mutable";
        case Mutability::Immutable: return "immutable";
        default: {
            return "mutability_UNKNOWN";
        }
    }
}

inline std::string meaning_from_primitive(const Primitive& prim) {
    switch(prim) {
        case Primitive::Free: return "free-pointer";
        case Primitive::Int: return "integer"; 
        case Primitive::Float: return "float";
        case Primitive::Char: return "char";
        case Primitive::Boolean: return "boolean";
        case Primitive::Ptr: return "pointer";
        default: {
            return "primitive_UNKNOWN";
        }
    }
}

inline Mutability mut_from_token(const Token& tok) {
    if(tok.match_keyword(Keyword::Mut)) {
        return Mutability::Mutable;
    }
    if(tok.match_keyword(Keyword::Let)) {
        return Mutability::Immutable;
    }
    ASSERT(false, std::format(
        "expected token that represents a variable declaration but got '{}'", 
        tok.value
    ));
}

inline Option<Primitive> try_primitive_from_ident(const Expr::Identifier& ident) {
    if(ident.matches("free")) return Primitive::Free;
    if(ident.matches("int")) return Primitive::Int; 
    if(ident.matches("float")) return Primitive::Float;
    if(ident.matches("ch")) return Primitive::Char;
    if(ident.matches("bool")) return Primitive::Boolean;
    if(ident.matches("ptr")) return Primitive::Ptr;
    return std::nullopt;
}

}

#endif // STMT_HPP_