#ifndef SEMANTICS_HPP_
#define SEMANTICS_HPP_

#include "sym.hpp"
#include "builtins.hpp"
#include <array>

using Tokenizer::bin_op_str;
using Tokenizer::assign_op_str;
using Tokenizer::un_op_str;

struct LiteralNode;
struct BinOpNode;
struct UnaryOpNode;
struct VarDeclarationNode;
struct VarTerminalNode;
struct AssignmentNode;
struct DerefAssignmentNode;
struct FnHeaderNode;
struct FnNode;
struct BlockNode;
struct LoopNode;
struct IfNode;
struct BreakNode;
struct ReturnNode;
struct FnCallNode;
struct ImportNode;
struct ExprNode;

struct SemanticVisitor {
    using BuiltIns = std::array<std::string, 1>;

    SymTable table;
    
    SemanticVisitor() : table() {}
    
    void include_builtins() {
        for (const auto& builtin : BUILTINS) {
            auto sym = sig_to_sym(builtin.signature);
            ASSERT(sym.has_value(), format("caught an invalid signature parsing attempt: '{}'", builtin.signature));
            
            SharedPtr<SymFunction> fn = std::make_shared<SymFunction>(std::move(sym.value()));
            table.insert_symbol(builtin.ident, std::move(fn));
        }
    };

    inline bool is_builtin(const Identifier& ident) {
        for(const auto& builtin_symbol : BUILTINS)
            if(ident.matches(builtin_symbol.ident)) return true;
        return false;
    };

    inline bool sema_type_cmp(Type& given, Type& expected) {
        return given.hash() == expected.hash();
    }
    
    SharedPtr<Type> sema_ptr_arithmetics(const BinOpKind& op, SharedPtr<Type>& lhs, SharedPtr<Type>& rhs);
    SharedPtr<Type> sema_process_type(const BinOpKind& op, SharedPtr<Type>& lhs, SharedPtr<Type>& rhs);
    
    bool sema_type_primitive_cmp(SharedPtr<Type>& ty, Primitive&& expected);
    bool sema_ptr_mut_within_assignment(Ptr<ExprNode>& expr);

    void sema_analyze(LiteralNode& lit);
    void sema_analyze(BinOpNode& bin);
    void sema_analyze(UnaryOpNode& un);
    void sema_analyze(VarTerminalNode& term);
    void sema_analyze(VarDeclarationNode& decl);
    void sema_analyze(FnHeaderNode& fn_header);
    void sema_analyze(FnNode& fn);
    void sema_analyze(BlockNode& block);
    void sema_analyze(LoopNode& loop);
    void sema_analyze(IfNode& cfn);
    void sema_analyze(BreakNode& brk);
    void sema_analyze(ReturnNode& ret);
    void sema_analyze(FnCallNode& fn_call);
    void sema_analyze(AssignmentNode& assignment);
    void sema_analyze(DerefAssignmentNode& deref_assignment);
    void sema_analyze(ImportNode& imprt);
};

#endif // SEMANTICS_HPP_