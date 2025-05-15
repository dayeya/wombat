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
    // Represents a label in the code (e.g. @main, .if)
    Label,
    // Copies a variable to another
    // Example: mut y: int = x;
    Copy,
    // Creates a temporary from an expression
    // Example: putnum(1 + 2) --> %t1 = 1 + 2
    Temp,
    // Pushes an argument to the stack
    // Example: push %1
    Push,
    // Pops a value into a target (e.g. a local)
    // Example: pop #[local(buffer)]
    Pop,
    // Calls a function with arguments
    // Example: call print 1
    Call,
    // Returns a value from a function
    // Example: ret %t1
    Ret,
    // Performs a system call
    // Example: syscall OUT
    Syscall,
    // Arithmetic operations
    Add,        // Adds two values (a + b)
    Sub,        // Subtracts two values (a - b)
    Mul,        // Multiplies two values (a * b)
    Div,        // Divides two values (a / b)
    FlooredDiv, // Integer division with floor (floor(a / b))
    Mod,        // Modulus (a % b)
    // Logical operations
    And,        // and: a and b
    Or,         // or: a or b
    // Bitwise operations
    BitXor,     // Bitwise XOR (a ^ b)
    BitAnd,     // Bitwise AND (a & b)
    BitOr,      // Bitwise OR (a | b)
    Shl,        // Bitwise shift left (a << b)
    Shr,        // Bitwise shift right (a >> b)
    // Comparison operations
    Eq,         // eq: a == b
    Lt,         // lt: a < b
    Le,         // le: a <= b
    NotEq,      // neq: a != b
    Ge,         // gw: a >= b
    Gt,         // gt: a > b
    Neg,        // neg: -a
    Not,        // not: true
    BitNot,     // bitwise(not): !a
    // No operation (placeholder)
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
            case OpCode::Label:       return "label";
            case OpCode::Copy:        return "copy";
            case OpCode::Temp:        return "temp";
            case OpCode::Push:        return "push";
            case OpCode::Pop:         return "pop";
            case OpCode::Call:        return "call";
            case OpCode::Ret:         return "ret";
            case OpCode::Syscall:     return "syscall";
            case OpCode::Add:         return "add";
            case OpCode::Sub:         return "sub";
            case OpCode::Mul:         return "mul";
            case OpCode::Div:         return "div";
            case OpCode::FlooredDiv:  return "floor";
            case OpCode::Mod:         return "mod";
            case OpCode::And:         return "and";
            case OpCode::Or:          return "or";
            case OpCode::BitXor:      return "bitwise(xor)";
            case OpCode::BitAnd:      return "bitwise(and)";
            case OpCode::BitOr:       return "bitwise(or)";
            case OpCode::BitNot:      return "bitwise(not)";
            case OpCode::Shl:         return "shl";
            case OpCode::Shr:         return "shr";
            case OpCode::Eq:          return "eq";
            case OpCode::Lt:          return "lt";
            case OpCode::Le:          return "le";
            case OpCode::NotEq:       return "neq";
            case OpCode::Ge:          return "ge";
            case OpCode::Gt:          return "gt";
            case OpCode::Neg:         return "neg";
            case OpCode::Not:         return "not";
            case OpCode::Nop:         return "nop";
            default:                  return "unknown";
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