#include "ast.hpp"
#include "sema_visitor.hpp"
#include "typing.hpp"

bool SemanticVisitor::sema_ptr_mut_within_assignment(Ptr<ExprNode>& expr) {
    switch(expr->id)
    {
        case NodeId::Term:
        {
            auto* var = dynamic_cast<VarTerminalNode*>(expr.get());
            ASSERT(table.sym_exists(var->ident), format("variable '{}' does not exist in the current scope", var->ident.as_str()));
            SharedPtr<Symbol> sym = table.fetch_symbol(var->ident);
            if (sym->sym_kind == SymKind::Var) {
                SharedPtr<VarSymbol> metadata = std::dynamic_pointer_cast<VarSymbol>(sym);
                if(metadata->mut != Mutability::Mutable) {
                    ASSERT(
                        false, 
                        format("'{}' is not mutable", var->ident.as_str())
                    );
                    return false;
                }
                return true;
            }
        }
        case NodeId::Un: 
        {
            auto* un = dynamic_cast<UnaryOpNode*>(expr.get());
            return sema_ptr_mut_within_assignment(un->lhs);
            break;
        }
        default: {
            ASSERT(false, "unreachable");
            return false;
        }
    }
}

bool SemanticVisitor::sema_type_primitive_cmp(
    SharedPtr<Type>& ty, 
    Primitive&& expected
) {
    if (ty->fam != TypeFamily::Primitive) {
        return false;
    }
    auto prim = std::dynamic_pointer_cast<PrimitiveType>(ty);
    return prim && prim->cmp(std::move(expected));
}

SharedPtr<Type> SemanticVisitor::sema_ptr_arithmetics(
    const BinOpKind& op, 
    SharedPtr<Type>& lhs, 
    SharedPtr<Type>& rhs
) {
    switch (op)
    {
        case BinOpKind::Add:
        {
            if (lhs->fam == TypeFamily::Pointer && rhs->fam == TypeFamily::Pointer) {
                ASSERT(false, "cannot add pointer to pointer");
                return nullptr;
            }
            if (
                lhs->fam == TypeFamily::Pointer && sema_type_primitive_cmp(rhs, Primitive::Int) ||
                rhs->fam == TypeFamily::Pointer && sema_type_primitive_cmp(lhs, Primitive::Int)
            ) {
                return (lhs->fam == TypeFamily::Pointer) ? lhs : rhs;
            }
            ASSERT(false, format("invalid pointer addition: '{}' + '{}'", lhs->as_str(), rhs->as_str()));
            return nullptr;
        }

        case BinOpKind::Sub:
        {   
            // Can only allow:
            // * ptr - ptr
            // * ptr - int
            if (lhs->fam == TypeFamily::Pointer && rhs->fam == TypeFamily::Pointer) {
                auto l_ptr = std::dynamic_pointer_cast<PointerType>(lhs);
                auto r_ptr = std::dynamic_pointer_cast<PointerType>(rhs);
                ASSERT(
                    sema_type_cmp(*l_ptr->underlying, *r_ptr->underlying),
                    format("cannot perform '{}' operation on '{}' and '{}'", bin_op_str(op), lhs->as_str(), rhs->as_str())
                );
                return std::make_shared<PrimitiveType>(Primitive::Int);
            }
            if(lhs->fam == TypeFamily::Pointer && sema_type_primitive_cmp(rhs, Primitive::Int)) {
                return lhs;
            }
            ASSERT(false, format("invalid pointer subtraction: '{}' - '{}'", lhs->as_str(), rhs->as_str()));
            return nullptr;
        }
        case BinOpKind::Eq:
        case BinOpKind::NotEq:
        case BinOpKind::Gt:
        case BinOpKind::Lt:
        case BinOpKind::Ge:
        case BinOpKind::Le:
        {
            if (lhs->fam == TypeFamily::Pointer && rhs->fam == TypeFamily::Pointer) {
                auto l_ptr = std::dynamic_pointer_cast<PointerType>(lhs);
                auto r_ptr = std::dynamic_pointer_cast<PointerType>(rhs);
                ASSERT(
                    sema_type_cmp(*l_ptr->underlying, *r_ptr->underlying),
                    format("invalid comparison of different types: '{}' with '{}'", lhs->as_str(), rhs->as_str())
                );
                return std::make_shared<PrimitiveType>(Primitive::Boolean);
            }
            ASSERT(false, format("invalid pointer comparison: '{}' vs '{}'", lhs->as_str(), rhs->as_str()));
            return nullptr;
        }
        default:
            ASSERT(false, format("unsupported pointer operation '{}'", bin_op_str(op)));
            return nullptr;
    }
}

SharedPtr<Type> SemanticVisitor::sema_process_type(
    const BinOpKind& op, 
    SharedPtr<Type>& lhs, 
    SharedPtr<Type>& rhs
) {
    if (lhs->fam == TypeFamily::Array || rhs->fam == TypeFamily::Array) {
        ASSERT(
            false,
            format("invalid expression: cannot use '{}' and '{}' in binary operation", lhs->as_str(), rhs->as_str())
        );
        return nullptr;
    }
    if (lhs->fam == TypeFamily::Pointer || rhs->fam == TypeFamily::Pointer) {
        return sema_ptr_arithmetics(op, lhs, rhs);
    }
    if(!sema_type_cmp(*lhs, *rhs)) {
        return nullptr;
    }
    switch (op) {
        case BinOpKind::Add:
        case BinOpKind::Sub:
        case BinOpKind::Div:
        case BinOpKind::Mul:
        case BinOpKind::Mod:
        case BinOpKind::Pow:
        case BinOpKind::FlooredDiv:
        case BinOpKind::BitAnd:
        case BinOpKind::BitOr:
        case BinOpKind::BitXor:
        {
            return lhs;
        }
        case BinOpKind::Shl:
        case BinOpKind::Shr:
        {
            if (sema_type_primitive_cmp(rhs, Primitive::Int)) {
                return std::make_shared<PrimitiveType>(Primitive::Int);
            }
            ASSERT(false, format("cannot shift type '{}'", lhs->as_str()));
            return nullptr;
        }
        case BinOpKind::Eq:
        case BinOpKind::NotEq:
        case BinOpKind::Lt:
        case BinOpKind::Gt:
        case BinOpKind::Le:
        case BinOpKind::Ge:
        case BinOpKind::And:
        case BinOpKind::Or:
        {
            return std::make_shared<PrimitiveType>(Primitive::Boolean);
        }
        default:
        {
            ASSERT(false, format("unreachable '{}'", bin_op_str(op)));
            return nullptr;
        }
    }
}

void SemanticVisitor::sema_analyze(LiteralNode& lit) {
    switch (lit.kind)
    {
        case LiteralKind::Int:
            lit.sema_type = std::make_shared<PrimitiveType>(Primitive::Int);
            break;
        case LiteralKind::Float:
            lit.sema_type = std::make_shared<PrimitiveType>(Primitive::Float);
            break;
        case LiteralKind::Char:
            lit.sema_type = std::make_shared<PrimitiveType>(Primitive::Char);
            break;
        case LiteralKind::Str:
            ASSERT(false, "unimplemented");
        case LiteralKind::Bool:
            lit.sema_type = std::make_shared<PrimitiveType>(Primitive::Boolean);
            break;
        default:
            UNREACHABLE();
    }
};

void SemanticVisitor::sema_analyze(BinOpNode& bin) {
    bin.lhs->analyze(*this);
    bin.rhs->analyze(*this);

    auto& lhs_type = bin.lhs->sema_type;
    auto& rhs_type = bin.rhs->sema_type;

    ASSERT(
        lhs_type && rhs_type, 
        "operands must be typed before binary operation.",
        FATAL_ERROR_PREFIX
    );

    auto processed_type = sema_process_type(bin.op, lhs_type, rhs_type);
    if(processed_type != nullptr) {
        bin.sema_type = processed_type;
    }
    else {
        ASSERT(
            false, 
            format(
                "mismatched types: '{} {} {}'", 
                lhs_type->as_str(), 
                bin_op_str(bin.op), 
                rhs_type->as_str()
            )
        );
    }
};

void SemanticVisitor::sema_analyze(UnaryOpNode& un) {
    un.lhs->analyze(*this);

    switch (un.op) {
        case UnOpKind::Neg:
        {
            if (
                sema_type_primitive_cmp(un.lhs->sema_type, Primitive::Int) ||
                sema_type_primitive_cmp(un.lhs->sema_type, Primitive::Float)
            ) {
                un.sema_type = un.lhs->sema_type;
                return;
            }
            ASSERT(false, format("cannot negate type '{}'", un.lhs->sema_type->as_str()));
        }

        case UnOpKind::Not:
        {
            if (sema_type_primitive_cmp(un.lhs->sema_type, Primitive::Boolean)) {
                un.sema_type = std::make_shared<PrimitiveType>(Primitive::Boolean);
                return;
            }
            ASSERT(false, format("cannot apply 'not' to type '{}'", un.lhs->sema_type->as_str()));
        }
        case UnOpKind::BitNot:
        {
            if (
                sema_type_primitive_cmp(un.lhs->sema_type, Primitive::Int) ||
                sema_type_primitive_cmp(un.lhs->sema_type, Primitive::Boolean) ||
                sema_type_primitive_cmp(un.lhs->sema_type, Primitive::Char)
            ) {
                un.sema_type = std::make_shared<PrimitiveType>(Primitive::Int);
                return;
            }
            ASSERT(false, format("cannot apply '!' to type '{}'", un.lhs->sema_type->as_str()));
        }
        case UnOpKind::AddrOf:
        {
            ASSERT(un.lhs->category == ValueCategory::LValue, format("cannot take address of r-value expression: '{}'", un.lhs->id_str()));
            un.sema_type = std::make_shared<PointerType>(un.lhs->sema_type);
            un.category = ValueCategory::RValue;
            break;
        }
        case UnOpKind::Dereference:
        {
            ASSERT(
                un.lhs->sema_type->is_ptr(), 
                format("cannot dereference non-pointer type: '{}'", un.lhs->sema_type->as_str())
            );
            ASSERT(
                un.lhs->category == ValueCategory::LValue, 
                format("cannot dereference r-value expression: '{}'", un.lhs->id_str())
            );
            un.category = ValueCategory::LValue;
            un.sema_type = (std::dynamic_pointer_cast<PointerType>(un.lhs->sema_type))->underlying;
            break;
        }
        default:
            UNREACHABLE();
    }
};

void SemanticVisitor::sema_analyze(VarTerminalNode& term) {
    ASSERT(
        table.sym_exists(term.ident), 
        format("'{}' was not declared in this scope.", term.ident.as_str())
    );

    SharedPtr<Symbol> sym = table.fetch_symbol(term.ident);
    switch (sym->sym_kind)
    {
        case SymKind::Fn:
            ASSERT(false, format("invalid expression: '{}' not a term.", term.ident.as_str()));
            break;
        case SymKind::Var:
        {
            SharedPtr<VarSymbol> metadata = std::dynamic_pointer_cast<VarSymbol>(sym);
            term.sema_type = metadata->type;
            term.category = ValueCategory::LValue;
            break;
        }
        default:
            ASSERT(false, "unreachable.");
    }
};

void SemanticVisitor::sema_analyze(AssignmentNode& assign) {
    ASSERT(
        table.sym_exists(assign.lvalue), 
        format("'{}' was not declared in this scope.", assign.lvalue.as_str())
    );
    SharedPtr<Symbol> sym = table.fetch_symbol(assign.lvalue);
    switch (sym->sym_kind)
    {
        case SymKind::Var:
        {
            SharedPtr<VarSymbol> metadata = std::dynamic_pointer_cast<VarSymbol>(sym);
            ASSERT(
                metadata->mut == Mutability::Mutable, 
                format("'{}' is not mutable.", assign.lvalue.as_str())
            );
            
            auto& expr = assign.rvalue;
            expr->analyze(*this);
        
            if(!sema_type_cmp(*expr->sema_type, *metadata->type)) {
                ASSERT(
                    false,
                    format(
                        "{} is of type: '{}', but got: '{}'", 
                        assign.lvalue.as_str(), 
                        metadata->type->as_str(),
                        expr->sema_type->as_str()
                    )
                );
                return;
            }
            break;
        }
        case SymKind::Fn:
        {
            ASSERT(
                false, 
                format("'{}' is a function and cannot be assigned to.", assign.lvalue.as_str())
            );
            break;
        }
        default:
            UNREACHABLE();
    }    
}

void SemanticVisitor::sema_analyze(DerefAssignmentNode& deref_assign) {
    deref_assign.lvalue->analyze(*this);
    deref_assign.rvalue->analyze(*this);

    ASSERT(
        deref_assign.lvalue->category == ValueCategory::LValue, 
        format("cannot dereference r-value expression: '{}'", deref_assign.lvalue->id_str())
    );
    sema_ptr_mut_within_assignment(deref_assign.lvalue);
    ASSERT(
        sema_type_cmp(*deref_assign.lvalue->sema_type, *deref_assign.rvalue->sema_type),
        format(
            "mismatched types: cannot assign '{}' to pointer of type '{}'", 
            deref_assign.rvalue->sema_type->as_str(), 
            deref_assign.lvalue->sema_type->as_str()
        )
    );
};

void SemanticVisitor::sema_analyze(VarDeclarationNode& decl) {
    ASSERT(
        !table.sym_exists(decl.info.ident), 
        format("'{}' was already declared in this scope.", decl.info.ident.as_str())
    );

    // If there is not initializer, add the symbol and quit.
    if(!decl.init) {
        table.insert_symbol(
            decl.info.ident,
            std::make_shared<VarSymbol>(VarSymbol{decl.info.type, decl.info.mut})
        );
        return;
    }

    auto& expr = decl.init;
    expr->analyze(*this);   
    ASSERT(expr->sema_type != nullptr, "[core:err]: expression must be bound to a type");

    if(!sema_type_cmp(*expr->sema_type, *decl.info.type)) {
        ASSERT(
            false,
            format("mismatched types: got '{}', expected: '{}'", expr->sema_type->as_str(), decl.info.type->as_str())
        );
        return;
    }

    table.insert_symbol(
        decl.info.ident,
        std::make_shared<VarSymbol>(VarSymbol{decl.info.type, decl.info.mut})
    );
}

void SemanticVisitor::sema_analyze(FnCallNode& fn_call) {
    ASSERT(
        table.sym_exists(fn_call.ident), 
        format("'{}' was not declared in this scope.", fn_call.ident.as_str())
    );
    SharedPtr<Symbol> sym = table.fetch_symbol(fn_call.ident);
    ASSERT(
        sym->sym_kind == SymKind::Fn,
        format("'{}' is not a function and cannot be called.", fn_call.ident.as_str())
    );
    SharedPtr<SymFunction> fn = std::dynamic_pointer_cast<SymFunction>(sym);

    ASSERT(
        fn_call.args.size() == fn->params.size(),
        format(
            "function '{}' takes {} parameters but {} arguments were provided",
            fn_call.ident.as_str(),
            fn->params.size(),
            fn_call.args.size()
        )
    );

    size_t cur = 0;
    while(cur < fn_call.args.size()) {
        auto& usr_arg = fn_call.args.at(cur);
        usr_arg->analyze(*this);

        auto& param = fn->params.at(cur);
        ASSERT(
            sema_type_cmp(*usr_arg->sema_type, *param.type),
            format(
                "in '{}', '{}' expects argument of type '{}', but got '{}'",
                fn_call.ident.as_str(),
                param.ident.as_str(),
                param.type->as_str(),
                usr_arg->sema_type->as_str()
            )
        );
        cur++;
    }

    fn_call.sema_type = fn->ret_type;
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
    ASSERT(!is_builtin(fn.header->name), "cannot redeclare a builtin function.");

    // Add a new function symbol to global scope.
    table.insert_symbol(
        fn.header->name, 
        std::make_shared<SymFunction>(
            SymFunction{fn.header->params, fn.header->ret_type}
        )
    );
    table.push_scope();
    fn.header->analyze(*this);
    fn.body->analyze(*this);
    table.pop_scope();
}

void SemanticVisitor::sema_analyze(ReturnNode& ret) {
    SharedPtr<SymFunction> fn = std::dynamic_pointer_cast<SymFunction>(table.fetch_symbol(ret.fn));

    if (sema_type_primitive_cmp(fn->ret_type, Primitive::Free)) 
    {
        if (ret.expr == nullptr) {
            return;
        } else {
            ret.expr->analyze(*this);
            ASSERT(
                false,
                format(
                    "'{}' has return type 'free' but got '{}'", 
                    ret.fn.as_str(), 
                    ret.expr->sema_type->as_str()
                )
            );
        }
    }
    else 
    {
        ASSERT(
            ret.expr != nullptr,
            format(
                "'{}' requires a return value of type '{}'", 
                ret.fn.as_str(), 
                fn->ret_type->as_str()
            )
        );
        ret.expr->analyze(*this);
        ASSERT(
            sema_type_cmp(*fn->ret_type, *ret.expr->sema_type),
            format(
                "Type mismatch in return: function '{}' expects '{}' but got '{}'", 
                ret.fn.as_str(),
                fn->ret_type->as_str(),
                ret.expr->sema_type->as_str()
            )
        );
    }
};

void SemanticVisitor::sema_analyze(BreakNode& brk) {
    // A function just to pass over this function when analyzing any block;
    while(true) break;
}

void SemanticVisitor::sema_analyze(LoopNode& loop) {
    loop.body->analyze(*this);
};

void SemanticVisitor::sema_analyze(ImportNode& import) {
    ASSERT(false, "NOT IMPLEMENTED");
};

void SemanticVisitor::sema_analyze(IfNode& cfn) {
    cfn.condition->analyze(*this);

    PrimitiveType boolean{Primitive::Boolean};
    if(!sema_type_cmp(*cfn.condition->sema_type, boolean)) {
        ASSERT(false, "'if' condition must have a 'bool' type");
    }

    table.push_scope();
    cfn.if_block->analyze(*this);
    table.pop_scope();
    
    if(cfn.else_block != nullptr) {
        table.push_scope();
        cfn.else_block->analyze(*this);
        table.pop_scope();
    }
}