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

    auto increase_depth = [&depth]() -> void { depth++; };
    auto decrease_depth = [&depth]() -> void { depth--; };
    auto inline_doc = [&stream, &depth](std::string&& line) -> void { stream << DOC << line << NEWLINE; };
    auto append = [&stream, &depth](std::string&& inst) -> void { stream << std::string(depth, TAB) << inst << NEWLINE; };

    for(auto& inst : insts) {
        switch(inst.op) 
        {
            case OpCode::Label: 
            {
                if(fn_label(inst)) {
                    append(format("@{}:", inst.dst.value()));
                }
                else {
                    // Label is a control flow label.
                    break;
                }
                increase_depth();
                break;
            }
            case OpCode::Alloc:
            {
                ASSERT(inst.parts.capacity() == 1, "unexpected number of operands for alloc instruction.");
                auto alloc_bytes = inst.parts.front()->as_str();
                append(format("#[stack_allocation({} bytes)]", alloc_bytes));
                append(format("alloc {}, {}", inst.dst.value(), alloc_bytes));
                break;
            }
            case OpCode::Assign:
            {
                ASSERT(inst.parts.capacity() == 1, "unexpected number of operands for assign instruction.");
                append(format("{} = {}", inst.dst.value(), inst.parts.front()->as_str()));
                break;
            }
            case OpCode::Push:
            {   
                ASSERT(inst.parts.capacity() == 1, "unexpected number of operands for push instruction.");
                auto& op = inst.parts.front();
                append(format("push {}", op->as_str()));
                break;
            }
            case OpCode::Pop: 
            {
                ASSERT(inst.parts.capacity() == 0, "unexpected number of operands for pop instruction.");
                append(format("pop |> {}", inst.dst.value()));
                break;
            }
            case OpCode::Call: 
            {   
                ASSERT(inst.parts.capacity() == 2, "unexpected number of operands for call instruction.");

                // Get the operand. (Represents the number of arguments.)
                auto dst = inst.dst;
                auto& fn = inst.parts.at(0);
                auto& args = inst.parts.at(1);

                if(dst.has_value()) {
                    append(format("{} = call {}, {}", dst.value(), fn->as_str(), args->as_str()));
                } else {
                    append(format("_ = call {}, {}", fn->as_str(), args->as_str()));
                }

                break;
            }
            case OpCode::Ret:
            {   
                ASSERT(inst.parts.capacity() == 1, "unexpected number of operands for ret instruction.");
                auto& op = inst.parts.front();
                append(format("ret {}", op->as_str()));
                break;
            }
            case OpCode::Add:
            case OpCode::Sub:
            case OpCode::Mul:
            case OpCode::Div:
            case OpCode::FlooredDiv:
            case OpCode::And:
            case OpCode::Or:
            case OpCode::Eq:
            case OpCode::NotEq:
            case OpCode::Le:
            case OpCode::Lt:
            case OpCode::Ge:
            case OpCode::Gt:
            case OpCode::BitAnd:
            case OpCode::BitXor:
            case OpCode::BitOr:
            {   
                ASSERT(
                    inst.parts.capacity() == 2, 
                    format("unexpected number of operands for {} instruction.", inst.op_as_str())
                );
                
                String dst = inst.dst.value();
                auto& lhs = inst.parts.at(0);
                auto& rhs = inst.parts.at(1);

                append(format("{} = {}: {}, {}", std::move(dst), inst.op_as_str(), lhs->as_str(), rhs->as_str()));
                break;
            }
            case OpCode::Not:
            case OpCode::Neg:
            case OpCode::BitNot:
            {
                ASSERT(
                    inst.parts.capacity() == 1, 
                    format("unexpected number of operands for {} instruction.", inst.op_as_str())
                );
                String dst = inst.dst.value();
                auto& lhs = inst.parts.front();
                append(format("{} = {}: {}", std::move(dst), inst.op_as_str(), lhs->as_str()));
                break;
            }
            default: 
                append(format("#[unhandled({})]", inst.op_as_str()));
        }
    }

    stream << NEWLINE;
    return stream.str();
}