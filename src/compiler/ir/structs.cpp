#include "structs.hpp"
#include <span>
#include <sstream>

std::string IrFn::dump() {
    ASSERT(
        insts.front().match_code(OpCode::Label),
        "a ir function must be preambled with a 'label' instruction."
    );

    // Setup a stream.
    int depth = 0;
    std::stringstream stream;
    auto append = [&stream, &depth](std::string&& inst) -> void { stream << std::string(depth, TAB) << inst; };
    auto increase_depth = [&depth]() -> void { depth++; };
    auto decrease_depth = [&depth]() -> void { depth--; };

    for(auto& inst : insts) {
        switch(inst.op) 
        {
            case OpCode::Label: 
            {
                if(fn_label(inst)) {
                    append(format("@{}:\n", inst.dst.value()));
                }
                else {
                    // Label is a control flow label.
                    break;
                }
                increase_depth();
                break;
            }
            case OpCode::Push:
            {   
                ASSERT(inst.parts.capacity() == 1, "unexpected number of operands for push instruction.");
                auto& op = inst.parts.front();
                append(format("push {}\n", op->as_str()));
                break;
            }
            case OpCode::Call: 
            {   
                ASSERT(inst.parts.capacity() == 1, "unexpected number of operands for push instruction.");

                // Get the operand. (Represents the number of arguments.)
                auto& op = inst.parts.front();
                ASSERT(op->kind == OpKind::Lit, "cannot push something that is not a literal.");

                auto* lop = static_cast<LitOp*>(op.get());
                append(format("call {}, {}\n", inst.dst.value(), lop->value));
                break;
            }
            case OpCode::Add:
            case OpCode::Sub:
            case OpCode::Mul:
            case OpCode::Div:
            {   
                ASSERT(
                    inst.parts.capacity() == 2, 
                    format("unexpected number of operands for {} instruction.", inst.op_as_str())
                );
                
                RawVal dst = inst.dst.value();
                auto& lhs = inst.parts.at(0);
                auto& rhs = inst.parts.at(1);

                append(format("{} = {}: {}, {}\n", std::move(dst), inst.op_as_str(), lhs->as_str(), rhs->as_str()));
                break;
            }
            case OpCode::Neg:
            case OpCode::BitNot:
            {
                ASSERT(
                    inst.parts.capacity() == 1, 
                    format("unexpected number of operands for {} instruction.", inst.op_as_str())
                );
                RawVal dst = inst.dst.value();
                auto& lhs = inst.parts.front();
                append(format("{} = {}: {}\n", std::move(dst), inst.op_as_str(), lhs->as_str()));
                break;
            }
            default: 
                ASSERT(false, format("cannot format a '{}' instruction. ", inst.op_as_str()));
        }
    }

    return stream.str();
}