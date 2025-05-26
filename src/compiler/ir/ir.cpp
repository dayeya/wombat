#include "ir.hpp"
#include <fstream>

using LoweredBlock = IrProgram::LoweredBlock;

void IrProgram::flatten_fn_call_from_stmt(LoweredBlock& block, Ptr<StmtNode>& fn_call) {
    auto* call = dynamic_cast<FnCallNode*>(fn_call.get());

    // Push all the arguments.
    for(int cur = call->args.capacity() - 1; cur >= 0; --cur)
    {
        auto& param = call->args.at(cur);

        Instruction::Parts ops;
        ops.push_back(flatten_expr(block, param));

        block.push_back(new_inst(OpCode::Push, std::nullopt, std::move(ops)));
    }

    Instruction::Parts ops;
    ops.push_back(new_var_op(call->ident.as_str()));
    ops.push_back(new_lit_op(std::format("{}", call->args.capacity()), LiteralKind::Int));

    auto call_inst = new_inst(
        OpCode::Call, 
        std::nullopt, 
        std::move(ops)
    );
    block.push_back(std::move(call_inst));
}

void IrProgram::flatten_ret_stmt(LoweredBlock& block, Ptr<StmtNode>& ret_stmt) {
    auto* ret = dynamic_cast<ReturnNode*>(ret_stmt.get());

    Instruction::Parts ops;
    if(ret->expr.get()) {
        ops.push_back(flatten_expr(block, ret->expr));
        cur_frame_size += ret->expr->sema_type->wsizeof();
    }

    ops.push_back(new_lbl_op(ret->fn.as_str()));
    block.push_back(new_inst(OpCode::Ret, ret->fn.as_str(), std::move(ops)));
}

void IrProgram::flatten_var_decl(LoweredBlock& block, Ptr<StmtNode>& var_decl) {
    auto* var = dynamic_cast<VarDeclarationNode*>(var_decl.get());

    Instruction::Parts ops;
    ops.push_back(new_lit_op(format("{}", var->info.type->wsizeof()), LiteralKind::Int));

    auto alloc = new_inst(
        OpCode::Alloc,
        var->info.ident.as_str(),
        std::move(ops)
    );

    cur_frame_size += var->info.type->wsizeof();
    block.push_back(std::move(alloc));

    if(var->initialized()) {
        Instruction::Parts ops;
        ops.push_back(flatten_expr(block, var->init));

        block.push_back(new_inst(
            OpCode::Assign,
            var->info.ident.as_str(),
            std::move(ops)
        ));
    }
}

void IrProgram::flatten_assign(LoweredBlock& ctx, Ptr<StmtNode>& assign) {
    auto* var = dynamic_cast<AssignmentNode*>(assign.get());

    Instruction::Parts ops;
    ops.push_back(flatten_expr(ctx, var->rvalue));

    ctx.push_back(new_inst(
        OpCode::Assign,
        var->lvalue.as_str(),
        std::move(ops)
    ));
}

void IrProgram::flatten_deref_assign(LoweredBlock& ctx, Ptr<StmtNode>& assign) {
    auto* deref = dynamic_cast<DerefAssignmentNode*>(assign.get());

    Ptr<Operand> address = flatten_expr_into_addr(ctx, deref->lvalue);
    Ptr<Operand> value = flatten_expr(ctx, deref->rvalue);

    Instruction::Parts ops;
    ops.push_back(std::move(address));
    ops.push_back(std::move(value));

    ctx.push_back(new_inst(
        OpCode::Store,
        std::nullopt,
        std::move(ops)
    ));
}

void IrProgram::flatten_only_if(LoweredBlock& ctx, Ptr<Operand>& op, Ptr<BlockNode>& if_block) {
    push_branch();
    String after = gen_branch_label("after");

    Instruction::Parts false_jump_ops;
    false_jump_ops.push_back(std::move(op));
    false_jump_ops.push_back(new_lbl_op(after));

    ctx.push_back(new_inst(
        OpCode::JmpFalse, 
        std::nullopt, 
        std::move(false_jump_ops)
    ));

    for (auto& inst : flatten_block(if_block)) {
        ctx.push_back(std::move(inst));
    }

    ctx.push_back(new_inst(
        OpCode::Label,
        std::move(after),
        {}
    ));
}

void IrProgram::flatten_if_and_else(
    LoweredBlock& ctx, 
    Ptr<Operand>& op, 
    Ptr<BlockNode>& if_block,
    Ptr<BlockNode>& else_block
) {
    push_branch();
    String else_label = gen_branch_label("else"), end_label = gen_branch_label("end");

    // JmpFalse to else_label if condition fails
    Instruction::Parts cond_jump_ops;
    cond_jump_ops.push_back(std::move(op));
    cond_jump_ops.push_back(new_lbl_op(else_label));

    ctx.push_back(new_inst(
        OpCode::JmpFalse,
        std::nullopt,
        std::move(cond_jump_ops)
    ));

    for (auto& inst : flatten_block(if_block)) {
        ctx.push_back(std::move(inst));
    }

    // Unconditional jump to end after the 'if' block
    Instruction::Parts end_jump_ops;
    end_jump_ops.push_back(new_lbl_op(end_label));

    ctx.push_back(new_inst(
        OpCode::Jmp,
        std::nullopt,
        std::move(end_jump_ops)
    ));
    ctx.push_back(new_inst(
        OpCode::Label,
        std::move(else_label),
        {}
    ));

    for (auto& inst : flatten_block(else_block)) {
        ctx.push_back(std::move(inst));
    }

    ctx.push_back(new_inst(
        OpCode::Label,
        std::move(end_label),
        {}
    ));
}

void IrProgram::flatten_branch(LoweredBlock& ctx, Ptr<StmtNode>& stmt) {
    auto* branch = dynamic_cast<IfNode*>(stmt.get());
    auto op = flatten_expr(ctx, branch->condition);

    if(branch->else_block == nullptr) {
        flatten_only_if(ctx, op, branch->if_block);
    } else {
        flatten_if_and_else(ctx, op, branch->if_block, branch->else_block);
    }
}

void IrProgram::flatten_loop_stmt(LoweredBlock& ctx, Ptr<StmtNode>& stmt) {
    auto* loop = dynamic_cast<LoopNode*>(stmt.get());

    push_loop();
    auto loop_labels = loop_stack.back();

    // Emit the start label
    ctx.push_back(new_inst(OpCode::Label, loop_labels.cnt, {}));

    // Flatten the loop block body
    for (auto& inst : flatten_block(loop->body)) {
        ctx.push_back(std::move(inst));
    }

    // Jump back to start of loop
    Instruction::Parts ops;
    ops.push_back(new_lbl_op(loop_labels.cnt));
    ctx.push_back(new_inst(OpCode::Jmp, std::nullopt, std::move(ops)));

    // Emit the end label (where `break` jumps to)
    ctx.push_back(new_inst(OpCode::Label, loop_labels.brk, {}));
    pop_loop();
}

void IrProgram::flatten_brk_stmt(LoweredBlock& ctx, Ptr<StmtNode>& break_stmt) {
    if(loop_stack.empty()) {
        ASSERT(false, "cannot break out a nonexistent loop.");
    } else {
        Instruction::Parts ops;
        ops.push_back(new_lbl_op(loop_stack.back().brk));
        ctx.push_back(new_inst(
            OpCode::Jmp,
            std::nullopt,
            std::move(ops)
        ));
    }
}

LoweredBlock IrProgram::flatten_block(Ptr<BlockNode>& block) {
    LoweredBlock body;
    body.reserve(block->children.capacity());

    for(auto& child : block->children)
    {
        switch(child->id) 
        {
            case NodeId::VarDecl: flatten_var_decl(body, child);          break;
            case NodeId::Assign: flatten_assign(body, child);             break;
            case NodeId::DerefAssign: flatten_deref_assign(body, child);  break;
            case NodeId::If: flatten_branch(body, child);                 break;
            case NodeId::FnCall: flatten_fn_call_from_stmt(body, child);  break;
            case NodeId::Return: flatten_ret_stmt(body, child);           break;
            case NodeId::Loop: flatten_loop_stmt(body, child);            break;
            case NodeId::Break: flatten_brk_stmt(body, child);            break;
            default: {
                ASSERT(
                    false,
                    format("cannot generate ir code from {}", child->id_str())
                );
            }
        }
    }

    return std::move(body);
}

Ptr<Operand> IrProgram::flatten_lit_expr(Ptr<ExprNode>& expr) {
    auto* lit = dynamic_cast<LiteralNode*>(expr.get());

    String buff{""};
    switch(lit->kind) {
        case LiteralKind::Int:
        case LiteralKind::Float:
        {
            buff = std::move(lit->str);
            break;
        }
        case LiteralKind::Char:
        {   
            char inner;
            if(lit->str.size() == 1) {
                // simple char like 'a'
                inner = lit->str[0];
            } else if(lit->str.size() == 2 && lit->str[0] == '\\') {
                switch (lit->str[1]) {
                    case 'n': inner = '\n'; break;
                    case 't': inner = '\t'; break;
                    case 'r': inner = '\r'; break;
                    case '0': inner = '\0'; break;
                    case '\\': inner = '\\'; break;
                    case '\'': inner = '\''; break;
                    case '\"': inner = '\"'; break;
                    default:
                        ASSERT(false, format("unsupported escape sequence '\\{}'", lit->str[1]));
                }
            } else {
                ASSERT(false, format("invalid char literal format: '{}'", lit->str));
            }

            buff = std::to_string(static_cast<int>(inner));
            break;
        }
        case LiteralKind::Bool: 
        {
            if(lit->str.compare("false") == 0) {
                buff = "0";
            }
            else if(lit->str.compare("true") == 0) {
                buff = "1";
            }
            else 
                UNREACHABLE();
            break;
        }
        default: 
            ASSERT(false, format("unreachable literal kind {}", lit_kind_str(lit->kind)));
    }

    auto operand = new_lit_op(std::move(buff), std::move(lit->kind));
    return std::move(operand);
}

Ptr<Operand> IrProgram::flatten_bin_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr) {
    auto* bin = dynamic_cast<BinOpNode*>(expr.get());
    auto lhs = flatten_expr(ctx, bin->lhs);
    auto rhs = flatten_expr(ctx, bin->rhs);

    cur_frame_size += bin->sema_type->wsizeof();

    // A temporary to hold the result.
    Ptr<TempOp> temp = new_tmp_op(push_temp());

    Instruction::Parts ops;
    ops.push_back(std::move(lhs));
    ops.push_back(std::move(rhs));

    // Push a new instruction into the current block.
    auto inst = new_inst(
        ir_op_from_bin(bin->op),
        temp->as_str(),
        std::move(ops)
    );
    ctx.push_back(std::move(inst));

    return std::move(temp);
}

Ptr<Operand> IrProgram::flatten_un_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr) {
    auto* un = dynamic_cast<UnaryOpNode*>(expr.get());

    // Handle special unary operations.
    if(un->op == UnOpKind::AddrOf) {
        ASSERT(un->lhs->id == NodeId::Term, "unreachable case: address of non-terminal node.");
        auto* term = dynamic_cast<VarTerminalNode*>(un->lhs.get());
        return new_addr_op(term->ident.as_str());
    }

    auto lhs = flatten_expr(ctx, un->lhs);
    cur_frame_size += un->sema_type->wsizeof();

    // A temporary to hold the result.
    Ptr<TempOp> temp = new_tmp_op(push_temp());

    Instruction::Parts ops;
    ops.push_back(std::move(lhs));

    // Push a new instruction into the current block.
    auto inst = new_inst(
        ir_op_from_un(un->op),
        temp->as_str(),
        std::move(ops)
    );
    ctx.push_back(std::move(inst));

    return std::move(temp);
}

Ptr<Operand> IrProgram::flatten_fn_call_from_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr) {
    auto* call = dynamic_cast<FnCallNode*>(expr.get());

    // Push all the arguments.
    for(auto it = call->args.rbegin(); it != call->args.rend(); ++it)
    {
        Instruction::Parts ops;
        ops.push_back(flatten_expr(ctx, *it));

        ctx.push_back(new_inst(OpCode::Push, std::nullopt, std::move(ops)));
        cur_frame_size += (*it)->sema_type->wsizeof();
    }
    
    // Increase the needed stack space by the size of the return value.
    cur_frame_size += call->sema_type->wsizeof();

    Instruction::Parts ops;
    ops.push_back(new_var_op(call->ident.as_str()));
    ops.push_back(new_lit_op(format("{}", call->args.capacity()), LiteralKind::Int));

    // Create a temp to call the value of the call.
    Ptr<TempOp> temp = new_tmp_op(push_temp());

    auto call_inst = new_inst(
        OpCode::Call, 
        temp->as_str(), 
        std::move(ops)
    );
    ctx.push_back(std::move(call_inst));

    return std::move(temp);
}

Ptr<Operand> IrProgram::flatten_terminal(LoweredBlock& ctx, Ptr<ExprNode>& expr) {
    auto* term = dynamic_cast<VarTerminalNode*>(expr.get());
    auto operand = new_var_op(std::move(term->ident.as_str()));
    return std::move(operand);
}

Ptr<Operand> IrProgram::flatten_expr_into_addr(LoweredBlock& ctx, Ptr<ExprNode>& expr) {
    switch(expr->id)
    {
        case NodeId::Term:
        {
            auto* var = dynamic_cast<VarTerminalNode*>(expr.get());
            return new_addr_op(var->ident.as_str());
        }
        case NodeId::Un: 
        {
            auto* un = dynamic_cast<UnaryOpNode*>(expr.get());
            if(un->op == UnOpKind::AddrOf) {
                return flatten_expr(ctx, un->lhs);
            } else if(un->op == UnOpKind::Dereference) {
                return flatten_expr(ctx, un->lhs);
            } else {
                ASSERT(false, format("unary operation '{}' is not addressable", un_op_str(un->op)));
            }
            break;
        }
        default:
            ASSERT(false, format("cannot flatten '{}' expression into address.", expr->id_str()));
    }
}

Ptr<Operand> IrProgram::flatten_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr) {
    switch(expr->id) 
    {
        case NodeId::Lit: return flatten_lit_expr(expr);
        case NodeId::Bin: return flatten_bin_expr(ctx, expr);
        case NodeId::Un: return flatten_un_expr(ctx, expr);
        case NodeId::Term: return flatten_terminal(ctx, expr);
        case NodeId::FnCall: return flatten_fn_call_from_expr(ctx, expr);
        default: 
        {
            ASSERT(
                false,
                format("cannot generate IR code from {}", expr->id_str())
            );
            return nullptr;
        }
    }
}

IrFn IrProgram::flatten_function(Ptr<FnNode>& fn) {
    IrFn flattened {
        fn->header->name.as_str()
    };

    // Push a label function definition.
    flattened.push_inst(new_inst(OpCode::Label, flattened.name, {}));

    // Push all the parameters.
    auto& params = fn->header->params;
    for(auto it = params.rbegin(); it != params.rend(); ++it) {
        Instruction::Parts ops;
        ops.push_back(new_lit_op(format("{}", (*it).type->wsizeof()), LiteralKind::Int));
        flattened.push_inst(new_inst(OpCode::Pop, (*it).ident.as_str(), std::move(ops)));
    }

    // Push all remaining instructions.
    for(auto& inst : flatten_block(fn->body)) {
        flattened.push_inst(std::move(inst));
    }

    flattened.space_occupied = cur_frame_size;
    return flattened;
};

void IrProgram::gen(AST& ast) {
    lowered_program.reserve(ast.functions.capacity());
    for(auto& fn : ast.functions) {
        lowered_program.push_back(flatten_function(fn));
        cur_frame_size = 0;
    }
}

void IrProgram::dump() {
    ASSERT(dumpable(), "cannot dump an ir which is not dumpable.");
    
    // Set up the path.
    std::ofstream file(
        transform_extension(fs::path{ src.value() })
    );

    for(auto& fn : lowered_program) {
        file << fn.dump();
    }
}