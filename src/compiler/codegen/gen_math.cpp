#include <fstream>
#include "gen.hpp"


void CodeGen::emit_add(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // loads both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    appendln("add rax, rbx");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_sub(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // load both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    appendln("sub rax, rbx");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_mul(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // load both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    appendln("xor rdx, rdx");
    appendln("imul rbx");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_div(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // load both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    appendln("; sign-extend rax. ");
    appendln("cqo");
    appendln("idiv rbx");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_mod(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // load both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));
    
    appendln("; sign-extend rax. ");
    appendln("cqo");
    appendln("idiv rbx");
    appendln(format("mov qword [rbp - {}], rdx", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_bitand(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // load both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    appendln("and rax, rbx");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_bitor(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // load both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    appendln("or rax, rbx");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_bitxor(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // load both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    appendln("xor rax, rbx");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_neg(Instruction& inst) {
    auto sym = inst.dst.value();
    auto& lhs = inst.parts.at(0);
    stack.allocate(sym, TEMP_SIZE);
    
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    appendln("neg rax");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_bitnot(Instruction& inst) {
    auto sym = inst.dst.value();
    auto& lhs = inst.parts.at(0);
    stack.allocate(sym, TEMP_SIZE);
    
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    appendln("not rax");
    appendln(format("mov qword [rbp - {}], rax", stack.offset(sym)));
    appendln("");
}

void CodeGen::emit_cmp(Instruction& inst) {
    auto sym = inst.dst.value();
    stack.allocate(sym, TEMP_SIZE);

    auto& lhs = inst.parts.at(0);
    auto& rhs = inst.parts.at(1);

    // loads both sides.
    load_operand(lhs, "rax", gain_symbol(lhs)); 
    load_operand(rhs, "rbx", gain_symbol(rhs));

    String set;
    switch (inst.op)
    {
        case OpCode::Eq:    set = "sete";  break; 
        case OpCode::NotEq: set = "setne"; break; 
        case OpCode::Lt:    set = "setl";  break; 
        case OpCode::Le:    set = "setle"; break; 
        case OpCode::Gt:    set = "setg";  break; 
        case OpCode::Ge:    set = "setge"; break; 
        default:
            UNREACHABLE();
    }

    appendln("cmp rax, rbx");
    appendln(format("{} al", set));
    appendln("movzx rax, al");

    size_t memsize = stack.memsize(sym);
    String nasm = format(
        "mov {} [rbp - {}], rax",
        mem_ident_from_size(memsize),
        stack.offset(sym)
    );
    appendln(std::move(nasm));
    appendln("");
}