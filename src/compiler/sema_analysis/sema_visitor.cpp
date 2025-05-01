#include "ast.hpp"
#include "sema_visitor.hpp"

void SemanticVisitor::sema_analyze(LiteralNode& lit) {
    ASSERT(false,"`NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(BinOpNode& bin) {
    ASSERT(false,"`NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(UnaryOpNode& un) {
    ASSERT(false,"`NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(VarTerminalNode& term) {
    ASSERT(false,"`NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(ArraySubscriptionNode& arr_sub) {
    ASSERT(false,"`NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(ReturnNode& ret) {
    ASSERT(false,"`NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(ImportNode& imprt) {
    ASSERT(false,"`NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(AssignmentNode& assign) {
    ASSERT(
        table.sym_exists(assign.ident), 
        format("'{}' was not declared in this scope.", assign.ident.as_str())
    );
    SharedPtr<Symbol> sym = table.fetch_symbol(assign.ident);
    switch (sym->sym_kind)
    {
        case SymKind::Var:
        {
            SharedPtr<VarSymbol> metadata = std::dynamic_pointer_cast<VarSymbol>(sym);
            ASSERT(
                metadata->mut == Mutability::Mutable, 
                format("'{}' is not mutable.", assign.ident.as_str())
            );
            TODO("Type check for assignment node is not implemeted.");
            break;
        }
        case SymKind::Fn:
        {
            ASSERT(
                false, 
                format("'{}' is a function and cannot be assigned to.", assign.ident.as_str())
            );
            break;
        }
        default:
        {
            ASSERT(false, "found an unsupported symbol kind.");
            break;
        }
    }    
}

void SemanticVisitor::sema_analyze(VarDeclarationNode& decl) {
    ASSERT(
        !table.sym_exists(decl.info.ident), 
        format("'{}' was already declared in this scope.", decl.info.ident.as_str())
    );
    // Type check...
    table.insert_symbol(
        decl.info.ident, 
        std::make_shared<VarSymbol>(
            VarSymbol{
                decl.info.type, 
                decl.info.mut
            }
        )
    );
}

void SemanticVisitor::sema_analyze(FnCallNode& fn_call) {
    ASSERT(
        table.sym_exists(fn_call.ident), 
        format("'{}' was not declared in this scope.", fn_call.ident.as_str())
    );
    TODO("Type check for function call is not implemented.");
};

void SemanticVisitor::sema_analyze(BlockNode& block) {
    for (auto& stmt : block.children) {
        if(stmt) stmt->analyze(*this);
    }
}

void SemanticVisitor::sema_analyze(FnHeaderNode& fn_header) {
    for(const Parameter& param : fn_header.params) {
        SharedPtr<VarSymbol> sym = std::make_shared<VarSymbol>(param.type, param.mut);
        table.insert_symbol(param.ident, std::move(sym));
    }
}

void SemanticVisitor::sema_analyze(FnNode& fn) {
    // Add a new function symbol to global scope.
    table.insert_symbol(
        fn.header->name, 
        std::make_shared<SymFunction>(
            SymFunction{
                fn.header->params, 
                fn.header->ret_type
            }
        )
    );
    table.push_scope();
    fn.header->analyze(*this);
    fn.body->analyze(*this);
    table.pop_scope();
}