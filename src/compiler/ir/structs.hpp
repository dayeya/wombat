#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include "token.hpp"
#include "builtins.hpp"
#include <format>

using RawVal = std::string;
using Tokenizer::LiteralKind;

CONST char TAB = '\t';
CONST char NEWLINE = '\n';

enum class OpCode: int {
    // Represents a label within the binary code.
    // 
    // E.g '@main' or control flow like: '.if'
    //
    Label,
    // Perform a copy of a variable.
    //
    // E.g `
    //      mut x: int = 5;
    //      mut y: int = x;
    //     `
    Copy,
    // Creation of a temporary.
    // 
    // E.g 'putnum(1 + 2) --> %t1 = 1 + 2'
    // 
    Temp,
    // Push an arguement.
    //
    // E.g `push %1`
    Push,
    // Pop a parameter.
    //
    // E.g `pop #[local(buffer)]`
    Pop,
    // Invoke a function with argument.
    // 
    // E.g `call print 1`
    Call,
    // Return from a function.
    // 
    // E.g `ret %t1`
    Ret,
    // Perform a syscall.
    // 
    // E.g 'syscall OUT'
    Syscall,
    // Perform an addition operation.
    Add,
    // Perform an subtraction operation.
    Sub,
    // Perform an multiplication operation.
    Mul,
    // Perform an division operation.
    Div,
    // Peforms a negation.
    Neg,
    // Perform a bitwise not.
    BitNot,
    // Misc
    Nop
};

enum class OpKind : int {
    // Operand is a literal.
    Lit,
    // Operand is variable name. 
    Sym,
    // Operand is a temporary.
    Temp
};

struct Operand {
    OpKind kind;

    ~Operand() = default;
    Operand(OpKind&& kind) : kind(std::move(kind)) {}

    virtual RawVal as_str() const = 0;
};

struct LitOp : public Operand {
    RawVal value;
    LiteralKind kind;

    LitOp() : Operand(OpKind::Lit), value{}, kind{} {}
    LitOp(RawVal&& value, LiteralKind&& kind) : Operand(OpKind::Lit), value(std::move(value)), kind(std::move(kind)) {}

    RawVal as_str() const {
        return value;
    }
};

struct VarOp : public Operand {
    RawVal name;

    VarOp() : Operand(OpKind::Sym), name{} {}
    VarOp(std::string&& name) : Operand(OpKind::Sym), name{std::move(name)} {}

    RawVal as_str() const {
        return name;
    }
};

struct TempOp : public Operand {
    size_t id;

    TempOp() : Operand(OpKind::Temp), id{0} {}
    TempOp(size_t id) : Operand(OpKind::Temp), id{std::move(id)} {}

    RawVal as_str() const {
        return std::format("%t{}", id);
    }
};

struct Instruction {
    using Parts = std::vector<Ptr<Operand>>;

    OpCode op;
    Option<RawVal> dst;
    Parts parts;

    Instruction(OpCode&& op, Option<RawVal>&& dst, Parts&& parts) 
        : op{std::move(op)},
          dst{std::move(dst)},
          parts{std::move(parts)} {}

    bool match_code(OpCode&& other) {
        return op == other;
    }

    inline std::string op_as_str() {
        switch(op) {
            case OpCode::Label:   return "label";
            case OpCode::Copy:    return "copy";
            case OpCode::Push:    return "push";
            case OpCode::Pop:     return "pop";
            case OpCode::Call:    return "call";
            case OpCode::Ret:     return "ret";
            case OpCode::Syscall: return "syscall";
            case OpCode::Add:     return "add";
            case OpCode::Sub:     return "sub";
            case OpCode::Mul:     return "mul";
            case OpCode::Div:     return "div";
            case OpCode::Neg:     return "neg";
            case OpCode::BitNot:  return "bitwise(not)";
            case OpCode::Nop:     return "nop";
            default: {
                return "unknown";
            }
        }
    }
};

inline Instruction new_inst(OpCode&& op, Option<RawVal>&& dst, Instruction::Parts&& parts) {
    return Instruction(std::move(op), std::move(dst), std::move(parts));
};

struct IrFn {
    using Container = std::vector<Instruction>;

    RawVal name;
    Container insts;

    IrFn(RawVal&& name) : name{std::move(name)}, insts() {}
    IrFn(RawVal&& name, Container insts) 
        : name{std::move(name)},
          insts{std::move(insts)} {}

    bool builtin() {
        for(const auto& builtin : BUILTINS) {
            if(name.compare(builtin.ident) == 0) return true;
        }
        return false;
    }
    
    // Is 'inst' a label that defines a start of a function?
    bool fn_label(Instruction& inst) {
        ASSERT(inst.match_code(OpCode::Label), "instruction must be of type 'label'");
        return name.compare(inst.dst.value()) == 0;
    }

    void push_inst(Instruction&& inst) {
        insts.push_back(std::move(inst));
    }

    // A string that represents the function.
    std::string dump();
};

#endif // STRUCTS_HPP