#ifndef SEMA_ANALYSIS_HPP_
#define SEMA_ANALYSIS_HPP_

#include <unordered_map>
#include "stmt.hpp"

enum class SymKind : int {
    // A symbol that refers to a defined function.
    // E.g 'fn int foo() { ... }'
    //             ^^^
    Fn,
    // A symbol that refers to a defined variable.
    // E.g 'int bar = 42';
    //          ^^^
    Var,
    // A symbol that refers to a defined type.
    // E.g 'struct Foo {}'
    //             ^^^
    CustomType
};

struct Symbol {
    SymKind sym_kind;
    
    Symbol() = default;
    Symbol(SymKind&& kind) 
        : sym_kind{std::move(kind)} {}

    virtual ~Symbol() = default;
};

struct VarSymbol : public Symbol {
    Mutability mut;
    SharedPtr<Type> type;

    VarSymbol(SharedPtr<Type> type, const Mutability& mut)
        : Symbol(SymKind::Var), type(std::move(type)), mut(mut) {}
};

struct SymFunction : public Symbol {
    using FnParams = std::vector<Declaration::Parameter>;

    FnParams params;
    SharedPtr<Type> ret_type;
    
    SymFunction(FnParams params, SharedPtr<Type> ret_type)
        : Symbol(SymKind::Fn), params(std::move(params)), ret_type(std::move(ret_type)) {}
};

struct Scope {
    using SymIdent = std::string;
    using _Table = std::unordered_map<SymIdent, SharedPtr<Symbol>>;

    _Table syms;
    SharedPtr<Scope> parent;

    Scope(SharedPtr<Scope> parent_scp = nullptr) 
        : syms(), parent(std::move(parent_scp)) {}

    bool sym_exists(const Identifier& ident) const {
        return syms.find(ident.as_str()) != syms.end();
    }

    SharedPtr<Symbol> get_symbol_in_self(const Identifier& ident) const {
        ASSERT(
            sym_exists(ident), 
            std::format("[fatal::err] {} does not exist in current scope.", ident.as_str()
        ));
        auto it = syms.find(ident.as_str());
        return it->second;
    }
};

struct SymTable {
    SharedPtr<Scope> cur;

    SymTable() 
        : cur(std::make_shared<Scope>()) {}

    void push_scope() {
        cur = std::make_shared<Scope>(cur);
    }

    void pop_scope() {
        ASSERT(cur->parent != nullptr, "[fatal::err] cannot pop the global scope.");
        cur = cur->parent;
    }

    bool sym_exists_within_current_scope(const Identifier& ident) const {
        return cur->sym_exists(ident);
    }

    bool sym_exists(const Identifier& ident) const {
        SharedPtr<Scope> scope = cur;
        while(!scope->sym_exists(ident)) {
            if(scope->parent == nullptr) {
                return false;
            }
            scope = scope->parent;
        }
        return true;
    } 

    SharedPtr<Symbol> fetch_symbol(const Identifier& ident) const {
        SharedPtr<Scope> scope = cur;
        while (scope) {
            if (scope->sym_exists(ident)) {
                return scope->get_symbol_in_self(ident);
            }
            scope = scope->parent;
        }
        ASSERT(false, std::format("{} does not exist in any scope", ident.as_str()));
        return nullptr;
    }

    void insert_symbol(const Identifier& ident, SharedPtr<Symbol> symbol) {
        cur->syms[ident.as_str()] = std::move(symbol);
    }
};

#endif // SEMA_ANALYSIS_HPP_