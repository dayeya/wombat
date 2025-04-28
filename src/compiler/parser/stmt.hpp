#ifndef STMT_HPP_
#define STMT_HPP_

#include <format>
#include <vector>
#include <list>
#include "expr.hpp"

using Expr::ExprKind;
using Expr::BaseExpr;
using Expr::Identifier;

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
    Ptr<BaseExpr> expr;

    Return(Ptr<BaseExpr> expr) 
        : Stmt(StmtKind::FnReturn), expr(std::move(expr)) {}
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

enum class Mutability : int { Immutable, Mutable };

enum class Primitive : int {
    // A free pointer. 
    // 
    // E.g 'fn free main() ... end'
    Free,
    // An integer.
    Int, 
    // A float.
    Float,
    // One byte char.
    Char,
    // A boolean (true | false)
    Boolean
};

enum class TypeFamily : int {
    // A primitive type.
    // E.g 'int', 'float', 'char', 'bool'
    Primitive,
    // A pointer type.
    Pointer,
    // An array type.
    Array
};

struct Type {
    TypeFamily fam;

    Type(TypeFamily family) : fam(family) {}

    virtual std::string as_str() = 0;
};

struct PrimitiveType : public Type {
    Primitive type;

    PrimitiveType(Primitive&& type)
        : Type(TypeFamily::Primitive), type{std::move(type)} {}

    std::string as_str() override {
        switch(type) {
            case Primitive::Int: return "int";
            case Primitive::Float: return "float";
            case Primitive::Char: return "char";
            case Primitive::Boolean: return "bool";
            case Primitive::Free: return "free";
            default: {
                return "primitive_UNKNOWN";
            }
        }
    }
};

struct PointerType : public Type {
    Ptr<Type> underlying;

    PointerType(Ptr<Type>&& type)
        : Type(TypeFamily::Pointer), underlying(std::move(type)) {}

    std::string as_str() override {
        return std::format("ptr<{}>", underlying->as_str());
    }
};

struct ArrayType : public Type {
    size_t array_size;
    Ptr<Type> underlying;

    ArrayType(size_t&& size, Ptr<Type>&& type)
        : Type(TypeFamily::Array), array_size(std::move(size)), underlying(std::move(type)) {}
    
    std::string as_str() override {
        return std::format("[{}]{}", array_size, underlying->as_str());
    }
};

struct Initializer {
    Ptr<BaseExpr> expr;
    Tokenizer::AssignOp assignment;

    Initializer(const Tokenizer::AssignOp& op, Ptr<BaseExpr> expr)
        : expr(std::move(expr)), assignment(std::move(op)) {}
};

struct Assignment : public Stmt {
    Expr::Identifier ident;
    Initializer init;

    Assignment(Expr::Identifier&& ident, Initializer&& init) 
        : Stmt(StmtKind::Assignment), ident(std::move(ident)), init(std::move(init)) {}
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

inline Option<Primitive> maybe_primitive(const Identifier& ident) {
    if(ident.matches("free")) return Primitive::Free;
    if(ident.matches("int")) return Primitive::Int; 
    if(ident.matches("float")) return Primitive::Float;
    if(ident.matches("ch")) return Primitive::Char;
    if(ident.matches("bool")) return Primitive::Boolean;
    return std::nullopt;
}

struct Var : public Stmt {
    Mutability mut;
    Identifier ident;
    Ptr<Type> type;
    Option<Initializer> init;

    Var(
        Mutability&& mut,
        Identifier&& ident,
        Ptr<Type>&& type,
        Option<Initializer>&& init
    ) : Stmt(StmtKind::Local),
        mut{std::move(mut)},
        type{std::move(type)},
        ident{std::move(ident)},
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
    Ptr<Type> type;

    Parameter(Mutability&& mut, Identifier&& ident, Ptr<Type>&& type)
        : mut{std::move(mut)}, type{std::move(type)}, ident{std::move(ident)} {}

    std::string as_str() {
        return "{" + std::format(
            "\n\tmut: {}, \n\ttype: {}, \n\tident: {}\n",
            meaning_from_mutability(mut),            
            type->as_str(),            
            ident.as_str()         
        ) + "}";
    }
};

struct FnSignature {
    using TypeString = std::string;
    using FnParamSignature = std::vector<TypeString>;

    Ptr<Type> ret_type;
    FnParamSignature argument_types;

    FnSignature() = default;
    FnSignature(FnParamSignature&& types, Ptr<Type>&& ret) 
        : argument_types{std::move(argument_types)}, ret_type{std::move(ret)} {}

    std::string as_str() {
        std::string params{""};
        for(int k = 0; k < argument_types.size(); ++k)
        {   
            params += argument_types.at(k);
            if(k != argument_types.size() - 1) {
                params += ", ";
            }
        }
        return std::format("fn {} ({})", ret_type->as_str(), params);
    }
};

struct FnHeader {
    using FnParams = std::vector<Parameter>;
    
    FnSignature sig;
    FnParams params;
    Identifier ident;

    FnHeader() = default;
    FnHeader(FnSignature& sig, FnParams& params, Identifier& ident) 
        : sig(std::move(sig)), params(std::move(params)), ident(std::move(ident)) {}
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