#include <fstream>
#include "gen.hpp"
#include "builtins.hpp"

namespace fs = std::filesystem;

Option<Register> CodeGen::register_for_arguement_pipelining() {
    // Set register order.
    const std::vector<Register> reg_order = {
        Register::Rdi, 
        Register::Rsi, 
        Register::Rdx, 
        Register::Rcx, 
        Register::R8, 
        Register::R9
    };

    // Find the first register that is available.
    for (const auto& reg : reg_order) {
        const std::string reg_name = reg_to_str(reg);
        if (!registers[reg_name]) {
            registers[reg_name] = true;
            return reg;
        }
    }
    log("register spilling is not implemented.");
    return std::nullopt;
}

void CodeGen::emit_header(IrProgram& program) {
    appendln("global _start");
    appendln("; #[extern(linkage)]");

    for(auto& builtin : BUILTINS) {
        appendln(format("extern {}", builtin.ident));
    }

    appendln("");
}

void CodeGen::emit_data_section(IrProgram& program) {
    appendln("section .data");
    appendln("");
}

void CodeGen::emit_text_section(IrProgram& program) {
    appendln("section .text");
    appendln("_start:");
    increase_depth();
    appendln("call main\n");
    appendln("; wombat --> exit(0)");
    append("mov rax, 60");
    append_inline_comment("emit syscall: exit");
    append("mov rdi, 0");
    append_inline_comment("exit code of 0. (success)");
    appendln("syscall\n");
    decrease_depth();

    for(auto& fn : program.lowered_program) {
        emit_function(fn);
    }
}

void CodeGen::load_op_into_reg(
    const Ptr<Operand>& op, 
    const String& owner, 
    const Register& reg
) {
    switch (op->kind)
    {
        case OpKind::Lit:
        {   
            appendln(format("mov {}, {}", reg_to_str(reg), op->as_str()));
            break;
        }
        case OpKind::Sym:
        case OpKind::Temp:
        {
            size_t offset = stack.offset(owner);
            size_t memsize = stack.memsize(owner);
            appendln(
                format(
                    "mov {}, {} [rbp - {}]",
                    reg_to_str(reg),
                    size_identifier_from_bytes(memsize),
                    offset
                )
            );
            break;
        }
        default:
            ASSERT(false, "unreachable");
    }
}

void CodeGen::emit_instruction(IrFn& func, Instruction& inst) {
    switch(inst.op) 
    {
        case OpCode::Alloc:
        {
            ASSERT(inst.parts.capacity() == 1, "unexpected number of ops for alloc instruction.");
            auto& op = inst.parts.front();
            auto var = inst.dst.value();
            size_t size = size_from_string(op->as_str());
            stack.allocate(std::move(var), std::move(size));
            appendln(format("; #[alloc({} bytes for {})]", size, var));
            break;
        }
        case OpCode::Assign:
        {
            ASSERT(inst.parts.capacity() == 1, "unexpected number of ops for assign instruction.");
            auto& op = inst.parts.front();
            auto& var = inst.dst.value();
            size_t offset = stack.offset(var);
            size_t memsize = stack.memsize(var);
            
            // load rhs value into rax
            load_op_into_reg(op, var, Register::Rax);

            // push it into the stack slot [rbp - offset]
            appendln(format(
                "mov {} [rbp - {}], rax\n",
                size_identifier_from_bytes(memsize),
                offset
            ));
            break;
        }
        case OpCode::Push:
        {
            ASSERT(inst.parts.capacity() == 1, "push expects exactly one operand.");

            auto& op = inst.parts.front();
            auto reg = register_for_arguement_pipelining();

            if(reg.has_value()) 
            {
                load_op_into_reg(op, op->as_str(), reg.value());
            }
            else
            {
                ASSERT(false, "argument piplining exceeded 6 arguments.");
            }

            break;
        }
        case OpCode::Call:
        {
            stack.align_stack();
            const String& fn_name = inst.parts[0]->as_str();

            /*
                size_t cnt = inst.parts.size() - 1;

                // TODO: allow to pass more than 6 arguments.
                ASSERT(cnt <= 6, format("cannot pass more than 6 arguments to {}", fn_name));

                for (size_t i = 0; i < cnt && i < 6; i++) 
                {
                    auto& param = inst.parts.at(i + 1);
                    size_t offset = stack.offset(param->as_str());
                    appendln(format("mov {}, [rbp - {}]", ARG_REG[i], offset));
                }

            */

            appendln(format("call {}", fn_name));
            break;
        }
        default: 
            appendln(format("; #[--unhandled--({})]", inst.op_as_str()));
    }
}

void CodeGen::emit_function(IrFn& func) {
    ASSERT(
        func.insts.front().match_code(OpCode::Label),
        "IR function must start with a Label instruction."
    );
    
    // Enter new function scope
    String fn_name = func.insts.front().dst.value();
    stack.enter_func(fn_name);

    // Function prologue.
    appendln(format("{}:", fn_name));
    increase_depth();
    appendln("push rbp");
    appendln("mov rbp, rsp");
    
    // Allocate memory for local variables.
    if(func.space_occupied > 0) {
        appendln(format("sub rsp, {}", func.space_occupied));
    }

    // delimiter for seperating the function body.
    appendln("");

    for (size_t i = 1; i < func.insts.size(); ++i) {
        emit_instruction(func, func.insts[i]);
    }

    appendln("");
    appendln("mov rsp, rbp");
    appendln("pop rbp");
    appendln("ret");
    stack.exit_func();
}

void CodeGen::assemble(IrProgram& program) {
    // Reset state
    raw_program.str("");
    raw_program.clear();
    depth = 0;

    emit_header(program);
    emit_data_section(program);
    emit_text_section(program);
}