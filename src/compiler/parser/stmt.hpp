#ifndef STMT_HPP_
#define STMT_HPP_

#include <format>
#include <vector>
#include <list>
#include "token.hpp"
#include "expr.hpp"
#include "typing.hpp"

using Tokenizer::Identifier;
using Expr::ExprKind;
using Expr::BaseExpr;

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

enum class ScopeDelimiter : int {
    // A scope delimiter of braces.
    //
    // E.g 'if true { block... }'
    Braces,
    // A scope delimiter of the end keyword.
    //
    // E.g 'fn int foo() [block] end'
    EndKw
};

enum class StmtKind : int {
    // Import a module.
    //
    // E.g 'import foo' or 'import @local.bar'
    Import,
    // A local variable declaration.
    //
    // E.g 'let foo: bool = True;
    Local,
    // A local variable assignment.
    //
    // E.g 'foo = 42;'
    Assignment,
    // A function declaration.
    //
    // E.g 'fn int32 foo(mut bar: int32) ... end'
    FnDefinition,
    // A return statement.
    //
    // E.g 'return foo;'
    FnReturn,
    // A statement expression like funcation or macro calls.
    // 
    // E.g 'foo();'
    Expr
};

struct Stmt {
    StmtKind kind;

    Stmt(StmtKind kind) : kind(kind) {}
    virtual ~Stmt() = default;
};

struct Import : public Stmt {
    Identifier ident;

    Import(Identifier ident) 
        : Stmt(StmtKind::Import), ident(std::move(ident)) {}
};

struct FnCall : public Stmt {
    // A wrapper for a function call expression.
    // E.g 'foo(42, bar)'
    Ptr<Expr::FnCall> inner_expr;

    FnCall(Ptr<Expr::FnCall>&& fn_call)
        : Stmt(StmtKind::Expr), inner_expr(std::move(fn_call)) {}
};

struct Return : public Stmt {
    Identifier from;
    Ptr<BaseExpr> expr;

    Return(Identifier from, Ptr<BaseExpr> expr) 
        : Stmt(StmtKind::FnReturn), expr(std::move(expr)), from(from) {}
};

struct Block {
    using StmtList = std::vector<Ptr<Stmt>>;
    
    // A scope is just a list of statements to be executed.
    StmtList stmts;

    Block() : stmts{} {}

    StmtList& as_list() {
        return stmts;
    }
};

inline std::string meaning_from_stmt_kind(const StmtKind& kind) {
    switch(kind) {
        case StmtKind::Local: return "stmt_LOCAL";
        case StmtKind::Import: return "stmt_IMPORT"; 
        case StmtKind::FnDefinition: return "stmt_FN_DEFINITION";
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

struct Initializer {
    Ptr<BaseExpr> expr;
    Tokenizer::AssignOp assignment;

    Initializer(const Tokenizer::AssignOp& op, Ptr<BaseExpr> expr)
        : expr(std::move(expr)), assignment(std::move(op)) {}
};

struct VarInfo {
    Mutability mut;
    Identifier ident;
    SharedPtr<Type> type;

    VarInfo(Mutability&& mut, Identifier&& ident, SharedPtr<Type>&& type)
        : mut(std::move(mut)), ident(std::move(ident)), type(std::move(type)) {}
};

struct Assignment : public Stmt {
    Identifier ident;
    Initializer init;

    Assignment(Identifier&& ident, Initializer&& init) 
        : Stmt(StmtKind::Assignment), ident(std::move(ident)), init(std::move(init)) {}
};

struct Var : public Stmt {
    VarInfo info;
    Option<Initializer> init;

    Var(
        Mutability&& mut,
        Identifier&& ident,
        Ptr<Type>&& type,
        Option<Initializer>&& init
    ) : Stmt(StmtKind::Local),
        info{std::move(mut), std::move(ident), std::move(type)},
        init{std::move(init)} {}

    bool is_initialized() const {
        return init.has_value();
    }

    const Ptr<Expr::BaseExpr>& initializer_expr() {
        return init.value().expr;
    }
};

struct Parameter {
    Mutability mut;
    Identifier ident;
    SharedPtr<Type> type;

    Parameter(Mutability&& mut, Identifier&& ident, SharedPtr<Type>&& type)
        : mut{std::move(mut)}, type{std::move(type)}, ident{std::move(ident)} {}

    std::string as_str() {
        return std::format("{}{}: {}", 
            (mut == Mutability::Mutable ? "mut " : ""), 
            ident.as_str(), 
            type->as_str()
        );
    }
};


struct FnHeader {
    using FnParams = std::vector<Parameter>;

    Identifier ident;
    FnParams params;
    SharedPtr<Type> ret_type;

    FnHeader() = default;
    FnHeader(Identifier& ident, FnParams& params, SharedPtr<Type>& ret_type) 
        : ident(std::move(ident)), params(std::move(params)),  ret_type(std::move(ret_type)){}
    
    std::string as_str() {
        std::string within_paren_str{""};
        for(int k = 0; k < params.size(); ++k)
        {   
            within_paren_str += params.at(k).as_str();
            if(k != params.size() - 1) {
                within_paren_str += ", ";
            }
        }
        return std::format("fn {} ({})", ret_type->as_str(), within_paren_str);
    }
};

struct Fn : public Stmt {
    using FnBody = Statement::Block;

    FnHeader header;
    FnBody body;

    Fn(FnHeader& header, FnBody& body)
        : Stmt(StmtKind::FnDefinition), header(std::move(header)), body(std::move(body)) {}
};

}

#endif // STMT_HPP_