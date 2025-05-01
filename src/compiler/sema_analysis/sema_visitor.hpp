#ifndef SEMANTICS_HPP_
#define SEMANTICS_HPP_

#include "sym.hpp"

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