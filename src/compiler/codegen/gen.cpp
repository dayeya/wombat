#include <fstream>
#include "gen.hpp"
#include "builtins.hpp"

namespace fs = std::filesystem;

// Return the next available argument register (used for parameter passing)
Option<Register> CodeGen::register_for_arguement_pipelining() {
    for (const auto& reg : abi_registers) {
        const String& reg_name = reg_to_str(reg);
        if (!register_map[reg_name]) {
            register_map[reg_name] = true;
            return reg;
        }
    }

    log("Register spilling is not implemented.");
    return std::nullopt;
}

void CodeGen::clean_registers(size_t passed_arguments) {
    // Clean the first 6 registers.
    size_t cur = 0;
    while(cur < passed_arguments && cur < abi_registers.size()) {
        clean_register(abi_registers.at(cur++));
    }
}

void CodeGen::load_operand(
    Ptr<Operand>& op, 
    String&& reg,
    Option<String> sym
) {
    switch (op->kind) {
        case OpKind::Lit: 
        {
            auto* lop = dynamic_cast<LitOp*>(op.get());
            appendln(format("mov {}, {}", reg, op->as_str()));
            break;
        }
        case OpKind::Sym:
        {
            size_t offset = stack.offset(sym.value());
            size_t memsize = stack.memsize(sym.value());
            appendln(format(
                "mov {}, {} [rbp - {}]", 
                reg, 
                mem_ident_from_size(memsize), 
                offset
            ));
            break;
        }
        case OpKind::Temp: 
        {
            size_t offset = stack.offset(sym.value());
            size_t memsize = stack.memsize(sym.value());
            appendln(format(
                "mov {}, {} [rbp - {}]", 
                reg, 
                mem_ident_from_size(memsize), 
                offset
            ));
            break;
        } 
        default: 
            UNREACHABLE();
    }
}

void CodeGen::set_abi_registers() {
    abi_registers = {
        Register::Rdi, 
        Register::Rsi, 
        Register::Rdx,
        Register::Rcx, 
        Register::R8, 
        Register::R9
    };
    register_map = {
        { "rdi", false },
        { "rsi", false },
        { "rdx", false },
        { "rcx", false },
        { "r8", false },
        { "r9", false }
    };
}

void CodeGen::assemble(IrProgram& program) {
    raw_program.str("");
    raw_program.clear();
    depth = 0;

    set_abi_registers();
    emit_header(program);
    emit_data_section(program);
    emit_text_section(program);
}