#include "ir.hpp"
#include <fstream>

using LoweredBlock = IrProgram::LoweredBlock;

void IrProgram::gen(AST& ast) {
    for(auto& fn : ast.functions) {
        lowered_program.push_back(flatten_function(fn));
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

    return flattened;
};

LoweredBlock IrProgram::flatten_block(Ptr<BlockNode>& block) {
    LoweredBlock body;
    body.reserve(block->children.capacity());

    for(auto& child : block->children)
    {
        switch(child->id) 
        {
            case NodeId::FnCall:
            {
                auto* call = dynamic_cast<FnCallNode*>(child.get());

                // Push all the arguments.
                for(int cur = call->args.capacity() - 1; cur >= 0; --cur)
                {
                    auto& param = call->args.at(cur);

                    Instruction::Parts ops;
                    ops.push_back(flatten_expr(body, param));

                    body.push_back(new_inst(OpCode::Push, std::nullopt, std::move(ops)));
                }

                Instruction::Parts ops;
                ops.push_back(new_lit_op(std::format("{}", call->args.capacity()), LiteralKind::Int));

                auto call_inst = new_inst(
                    OpCode::Call, 
                    call->ident.as_str(), 
                    std::move(ops)
                );
                body.push_back(std::move(call_inst));
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

Ptr<Operand> IrProgram::flatten_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr) {
    switch(expr->id) 
    {
        case NodeId::Lit: 
        {
            auto* lit = dynamic_cast<LiteralNode*>(expr.get());
            auto operand = new_lit_op(std::move(lit->str), std::move(lit->kind));
            return std::move(operand);
        }
        case NodeId::Bin:
        {
            auto* bin = dynamic_cast<BinOpNode*>(expr.get());
            auto lhs = flatten_expr(ctx, bin->lhs);
            auto rhs = flatten_expr(ctx, bin->rhs);

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
        case NodeId::Un:
        {
            auto* un = dynamic_cast<UnaryOpNode*>(expr.get());
            auto lhs = flatten_expr(ctx, un->lhs);

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