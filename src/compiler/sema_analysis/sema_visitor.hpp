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
struct ArraySubscriptionNode;
struct AssignmentNode;
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
        SharedPtr<PrimitiveType> type = std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Free});
        SharedPtr<SymFunction> fn = std::make_shared<SymFunction>(SymFunction{
            {
                Declaration::Parameter(
                    Mutability::Immutable, 
                    Identifier{"out"}, 
                    std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Char})
                )
            },
            std::move(type)
        });
        table.insert_symbol(BUILTINS.at(0).ident, std::move(fn));
        
        type = std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Free});
        fn = std::make_shared<SymFunction>(SymFunction{
            {
                Declaration::Parameter(
                    Mutability::Immutable, 
                    Identifier{"num"}, 
                    std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Int})
                )
            },
            std::move(type)
        });
        table.insert_symbol(BUILTINS.at(1).ident, std::move(fn));
    };

    inline bool is_builtin(const Identifier& ident) {
        for(const auto& builtin_symbol : BUILTINS)
            if(ident.matches(builtin_symbol.ident)) return true;
        return false;
    };

    inline bool sema_type_cmp(Type& given, Type& expected) {
        return given.hash() == expected.hash();
    }

    bool sema_is_lval(Ptr<ExprNode>& expr);
    bool sema_is_rval(Ptr<ExprNode>& expr);
    
    // Compares the inner primitive type with 'expected'. 
    bool sema_type_primitive_cmp(SharedPtr<Type>& ty, Primitive&& expected);

    // Checks for possible pointer arithmetics.
    // *returns* The type of the result.
    SharedPtr<Type> sema_ptr_arithmetics(const BinOpKind& op, SharedPtr<Type>& lhs, SharedPtr<Type>& rhs);

    // Checks for valid compatibility between `lhs` and `rhs`.
    // *returns* The type of the result.
    SharedPtr<Type> sema_process_type(const BinOpKind& op, SharedPtr<Type>& lhs, SharedPtr<Type>& rhs);

    void sema_analyze(LiteralNode& lit);
    void sema_analyze(BinOpNode& bin);
    void sema_analyze(UnaryOpNode& un);
    void sema_analyze(VarTerminalNode& term);
    void sema_analyze(VarDeclarationNode& decl);
    void sema_analyze(ArraySubscriptionNode& arr_sub);
    void sema_analyze(FnHeaderNode& fn_header);
    void sema_analyze(FnNode& fn);
    void sema_analyze(BlockNode& block);
    void sema_analyze(LoopNode& loop);
    void sema_analyze(IfNode& cfn);
    void sema_analyze(BreakNode& brk);
    void sema_analyze(ReturnNode& ret);
    void sema_analyze(FnCallNode& fn_call);
    void sema_analyze(AssignmentNode& assignment);
    void sema_analyze(ImportNode& imprt);
};

#endif // SEMANTICS_HPP_