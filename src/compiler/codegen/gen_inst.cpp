#include <fstream>
#include "gen.hpp"

void CodeGen::emit_alloc(Instruction& inst) {
    auto ident = inst.dst.value();
    auto& op = inst.parts.front();

    ASSERT(op->kind == OpKind::Lit, "alloc must provide a size.");
    size_t size = std::stoull(op->as_str());
    stack.allocate(ident, size);

    String doc = format("; '{}' allocation of {} bytes", ident, size);
    appendln(std::move(doc));
}

void CodeGen::emit_assign(Instruction& inst) {
    auto ident = inst.dst.value();
    auto& op = inst.parts.front();

    size_t offset = stack.offset(ident);
    size_t memsize = stack.memsize(ident);

    // load it into "rax", if op is a temp, we free it.
    load_operand(op, "rax", gain_symbol(op));

    switch(memsize)
    {
        case 1: {
            String nasm = format(
                "mov {} [rbp - {}], al", 
                mem_ident_from_size(memsize), 
                offset
            );
            appendln(std::move(nasm));
            break;
        }
        case 8: {
            String nasm = format(
                "mov {} [rbp - {}], rax", 
                mem_ident_from_size(memsize), 
                offset
            );
            appendln(std::move(nasm));
            break;
        }
        default:
            log(format("'{}' is of size {}, therefore we dont support it.", ident, memsize));
    }
}

void CodeGen::emit_push(Instruction& inst) {
    // Why does the push instruction does not hold a destination?
    // A push operation pushes an operand.
    // That which can be of various kinds.
    auto& op = inst.parts.front();
    Option<Register> unoccupied_register = register_for_arguement_pipelining();

    if(!unoccupied_register.has_value()) {
        // Pass it through the stack.
        load_operand(op, "rax", gain_symbol(op));
        appendln("push rax");

        AsmStackFrame& fm = stack.get_current();
        fm.extra_arguments++; 
    } else {
        String reg = reg_to_str(unoccupied_register.value());
        load_operand(op, std::move(reg), gain_symbol(op));
    }
}

void CodeGen::emit_ret(Instruction& inst) {
    auto& op = inst.parts.at(0);
    auto& label = inst.parts.at(1);
    load_operand(op, "rax", gain_symbol(op));
    appendln(format("jmp .end_{}", label->as_str()));
}

void CodeGen::emit_pop(Instruction& inst) {
    auto ident = inst.dst.value();
    auto* size_op = dynamic_cast<LitOp*>(inst.parts.front().get());
    size_t size = std::stoull(size_op->value);

    stack.allocate(ident, size);
    size_t offset = stack.offset(ident);
    size_t memsize = stack.memsize(ident);

    if (argument_position < abi_registers.size()) 
    {
        Register slot = abi_registers.at(argument_position);
        String reg = reg_to_str(slot);
        String nasm = format(
            "mov {} [rbp - {}], {}", 
            mem_ident_from_size(memsize), 
            offset, 
            register_variant_from_size(slot, memsize)
        );
        appendln(std::move(nasm));
    } 
    else
    {
        size_t stack_offset = 16 + 8 * (argument_position - abi_registers.size());
        appendln(format("mov rax, [rbp + {}]", stack_offset));
        appendln(format("mov qword [rbp - {}], rax", offset));
    }
    // Update position for next arguement.
    argument_position++;
}

void CodeGen::emit_call(Instruction& inst) {
    stack.align_stack();

    auto sym = inst.dst;
    auto& name_op = inst.parts.at(0); 
    auto& args_op = inst.parts.at(1); 

    // emit a call.
    appendln(format("call {}", name_op->as_str()));

    // store the value of the function.
    if(sym.has_value()) {
        // temporary allocation.
        stack.allocate(sym.value(), TEMP_SIZE);
        String nasm = format("mov qword [rbp - {}], rax", stack.offset(sym.value()));
        appendln(std::move(nasm));
    }

    // clean the occupied register.
    size_t passed_arguments = std::stoull(args_op->as_str());
    clean_registers(passed_arguments);

    // Clean the stack if extra arguements used.
    AsmStackFrame& fm = stack.get_current();
    if (fm.extra_arguments > 0) {
        size_t cleanup_bytes = fm.extra_arguments * 8;
        appendln(format("add rsp, {}", cleanup_bytes));
        fm.extra_arguments = 0;
    }
}

void CodeGen::emit_label(Instruction& inst) {
    auto addr = inst.dst.value();
    decrease_depth();
    appendln(format("{}:", addr));
    increase_depth();
}

void CodeGen::emit_jmp(Instruction& inst) {
    auto& label_op = inst.parts.front();
    appendln(format("jmp {}", label_op->as_str()));    
}

void CodeGen::emit_jmp_false(Instruction& inst) {
    auto& condition_op = inst.parts.at(0);
    auto& addr_op = inst.parts.at(1);

    load_operand(condition_op, "rax", gain_symbol(condition_op));
    appendln("cmp rax, 0");
    appendln(format("je {}", addr_op->as_str()));
}

void CodeGen::emit_instruction(IrFn& func, Instruction& inst) {
    switch (inst.op) {
        case OpCode::Label: {
            emit_label(inst);
            break;
        }
        case OpCode::Alloc: 
        {
            emit_alloc(inst);
            break;
        }
        case OpCode::Assign: 
        {
            emit_assign(inst);
            break;
        }
        case OpCode::Push: 
        {
            emit_push(inst);
            break;
        }
        case OpCode::Pop:
        {
            emit_pop(inst);
            break;
        }
        case OpCode::Call:
        {
            emit_call(inst);
            break;
        }
        case OpCode::Ret: 
        {
            emit_ret(inst);
            break;
        }
        case OpCode::Add:
        {
            emit_add(inst);
            break;
        }
        case OpCode::Sub:
        {
            emit_sub(inst);
            break;
        }
        case OpCode::Mul:
        {
            emit_mul(inst);
            break;
        }
        case OpCode::Div:
        {
            emit_div(inst);
            break;
        }
        case OpCode::Mod:
        {
            emit_mod(inst);
            break;
        }
        case OpCode::BitAnd:
        {
            emit_bitand(inst);
            break;
        }
        case OpCode::BitXor:
        {
            emit_bitxor(inst);
            break;
        }
        case OpCode::BitOr:
        {
            emit_bitor(inst);
            break;
        }
        case OpCode::Neg:
        {
            emit_neg(inst);
            break;
        }
        case OpCode::BitNot:
        {
            emit_bitnot(inst);
            break;
        }
        case OpCode::And:
        case OpCode::Or:
        {
            emit_logical_binary_op(inst);
            break;
        }
        case OpCode::Not:
        {
            emit_logical_not(inst);
            break;
        }
        case OpCode::Eq:
        case OpCode::NotEq:
        case OpCode::Le:
        case OpCode::Lt:
        case OpCode::Ge:
        case OpCode::Gt:
        { 
            emit_cmp(inst);
            break;
        }
        case OpCode::Jmp: {
            emit_jmp(inst);
            break;
        }
        case OpCode::JmpFalse: {
            emit_jmp_false(inst);
            break;
        }
        default:
            appendln(format("; #[--unhandled--({})]", inst.op_as_str()));
    }
}

void CodeGen::emit_function(IrFn& func) {
    ASSERT(
        func.insts.front().match_code(OpCode::Label), 
        "function must begin with a label."
    );
    stack.enter_func(func.name);

    appendln(format("\n; FUNC {} START_IMPL", func.name));
    appendln(format("{}:", func.name));
    increase_depth();
    appendln("push rbp");
    appendln("mov rbp, rsp");

    if (func.space_occupied > 0) {
        appendln(format("sub rsp, {}", align_to(func.space_occupied, 16)));
    }

    appendln("");
    for (size_t i = 1; i < func.insts.size(); ++i) {
        emit_instruction(func, func.insts[i]);
    }

    decrease_depth();
    appendln("");
    appendln(format(".end_{}:", func.name));
    increase_depth();
    appendln("mov rsp, rbp");
    appendln("pop rbp");
    appendln("ret");

    decrease_depth();
    appendln(format("; FUNC {} END_IMPL", func.name));

    stack.exit_func();
}

void CodeGen::emit_header(IrProgram& program) {
    appendln("global _start");
    appendln("; #[extern(linkage)]");
    for (const auto& builtin : BUILTINS) {
        appendln(format("extern {}", builtin.ident));
    }
    appendln("");
}

void CodeGen::emit_data_section(IrProgram&) {
    appendln("section .data");
    appendln("");
}

void CodeGen::emit_text_section(IrProgram& program) {
    appendln("section .text");
    appendln("_start:");
    increase_depth();
    appendln("call main");
    appendln("mov rax, 60 ; emit syscall: exit");
    appendln("mov rdi, 0 ; exit code of 0 (success)");
    appendln("syscall");
    decrease_depth();

    for (auto& fn : program.lowered_program) {
        emit_function(fn);
        argument_position = 0;
    }
}