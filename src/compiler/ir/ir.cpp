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
    ops.push_back(flatten_expr(block, ret->expr));

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

void IrProgram::flatten_assignment(LoweredBlock& ctx, Ptr<StmtNode>& assign) {
    auto* var = dynamic_cast<AssignmentNode*>(assign.get());

    Instruction::Parts ops;
    ops.push_back(flatten_expr(ctx, var->expr));

    ctx.push_back(new_inst(
        OpCode::Assign,
        var->ident.as_str(),
        std::move(ops)
    ));
}

void IrProgram::flatten_if_stmt(LoweredBlock& ctx, Ptr<StmtNode>& stmt) {
    auto* control_flow = dynamic_cast<IfNode*>(stmt.get());
    ctx.push_back(new_inst(OpCode::Label, "", {}));
}

LoweredBlock IrProgram::flatten_block(Ptr<BlockNode>& block) {
    LoweredBlock body;
    body.reserve(block->children.capacity());

    for(auto& child : block->children)
    {
        switch(child->id) 
        {
            case NodeId::VarDecl: 
            {
                flatten_var_decl(body, child);
                break;
            }
            case NodeId::Assign:
            {
                flatten_assignment(body, child);
                break;
            }
            case NodeId::If: 
            {
                flatten_if_stmt(body, child);
                break;
            }
            case NodeId::FnCall:
            {
                flatten_fn_call_from_stmt(body, child);
                break;
            }
            case NodeId::Return:
            {
                flatten_ret_stmt(body, child);
                break;
            }
            default: 
            {
                ASSERT(
                    false,
                    format("cannot generate IR code from {}", child->id_str())
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
        case LiteralKind::Bool:
        {
            buff = std::move(lit->str);
            break;
        }
        case LiteralKind::Char:
        {
            buff = std::format("\'{}\'", lit->str);
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
    for(int cur = call->args.capacity() - 1; cur >= 0; --cur)
    {
        auto& param = call->args.at(cur);

        Instruction::Parts ops;
        ops.push_back(flatten_expr(ctx, param));

        ctx.push_back(new_inst(OpCode::Push, std::nullopt, std::move(ops)));
        cur_frame_size += param->sema_type->wsizeof();
    }
    
    // Increase the needed stack space by the size of the return value.
    cur_frame_size += call->sema_type->wsizeof();

    Instruction::Parts ops;
    ops.push_back(new_var_op(call->ident.as_str()));
    ops.push_back(new_lit_op(std::format("{}", call->args.capacity()), LiteralKind::Int));

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
    for(auto& param : fn->header->params) {
        flattened.push_inst(new_inst(OpCode::Pop, param.ident.as_str(), {}));
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