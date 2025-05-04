#ifndef SEMANTICS_HPP_
#define SEMANTICS_HPP_

#include "sym.hpp"

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
struct ReturnNode;
struct FnCallNode;
struct ImportNode;

struct SemanticVisitor {
    SymTable table;
    
    SemanticVisitor() : table() {}
    
    inline bool sema_type_cmp(const Type& ty1, const Type& ty2) {
        return ty1.hash() == ty2.hash();
    }
    
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
    void sema_analyze(ReturnNode& ret);
    void sema_analyze(FnCallNode& fn_call);
    void sema_analyze(AssignmentNode& assignment);
    void sema_analyze(ImportNode& imprt);
};

#endif // SEMANTICS_HPP_