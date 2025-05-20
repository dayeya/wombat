#include <fstream>
#include "gen.hpp"
#include "builtins.hpp"

void CodeGen::emit_alloc(Instruction& inst) {
    auto ident = inst.dst.value();
    auto& op = inst.parts.front();

    ASSERT(op->kind == OpKind::Lit, "alloc must provide a size.");
    size_t size = std::stoull(op->as_str());
    stack.allocate(ident, size);

    String doc = format("; '{}' allocation of {} ", ident, size);
    appendln(std::move(doc));
}

void CodeGen::emit_assign(Instruction& inst) {
    auto ident = inst.dst.value();
    auto& op = inst.parts.front();

    size_t offset = stack.offset(ident);
    size_t memsize = stack.memsize(ident);

    if(memsize != 8) {
        std::printf("we dont support the use of non-8 byte types, got %ld", memsize);
        UNREACHABLE();
    }
    
    // load it into "rax", if op is a temp, we free it.
    load_operand(op, "rax", gain_symbol(op));
    appendln(format("mov {} [rbp - {}], rax", mem_ident_from_size(memsize), offset));
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
    auto& op = inst.parts.front();
    load_operand(op, "rax", gain_symbol(op));
}

void CodeGen::emit_pop(Instruction& inst) {
    auto ident = inst.dst.value();
    auto* size_op = dynamic_cast<LitOp*>(inst.parts.front().get());

    size_t size = std::stoull(size_op->value);
    stack.allocate(ident, size);
    size_t offset = stack.offset(ident);

    if (argument_position < abi_registers.size()) 
    {
        String reg = reg_to_str(abi_registers.at(argument_position));
        appendln(format("mov qword [rbp - {}], {}", offset, reg));
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

void CodeGen::emit_instruction(IrFn& func, Instruction& inst) {
    switch (inst.op) {
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
        default:
            appendln(format("; #[--unhandled--({})]", inst.op_as_str()));
    }
}

void CodeGen::emit_function(IrFn& func) {
    ASSERT(
        func.insts.front().match_code(OpCode::Label), 
        "function must begin with a label."
    );
    String fn_name = func.insts.front().dst.value();
    stack.enter_func(fn_name);

    appendln(format("{}:", fn_name));
    increase_depth();
    appendln("push rbp");
    appendln("mov rbp, rsp");

    if (func.space_occupied > 0) {
        appendln(format("sub rsp, {}", func.space_occupied));
    }

    appendln("");
    for (size_t i = 1; i < func.insts.size(); ++i) {
        emit_instruction(func, func.insts[i]);
    }

    appendln("");
    appendln("mov rsp, rbp");
    appendln("pop rbp");
    appendln("ret");

    decrease_depth();
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
    appendln("");
    appendln("mov rax, 60 ; emit syscall: exit");
    appendln("mov rdi, 0 ; exit code of 0 (success)");
    appendln("syscall");
    decrease_depth();

    for (auto& fn : program.lowered_program) {
        emit_function(fn);
        argument_position = 0;
    }
}