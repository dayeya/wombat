#ifndef IR_HPP_
#define IR_HPP_

#include "ast.hpp"
#include "alias.hpp"
#include "typing.hpp"
#include "structs.hpp"
#include <filesystem>

namespace fs = std::filesystem;

class IrProgram {
public:
    using LoweredBlock = std::vector<Instruction>;
    using LoweredProgram = std::vector<IrFn>;

    Option<StrLoc> src;
    LoweredProgram lowered_program;

    IrProgram() : src{std::nullopt}, lowered_program{} {}
    IrProgram(StrLoc name) : src{name}, lowered_program{} {}

    // Writes a textual structure of the 'IR' into a file.
    void dump();

    // Generate an 'IR' from an ast.
    void gen(AST& ast);

private:
    size_t temp_counter = 0;
    size_t loop_counter = 0;
    size_t branch_counter = 0;

    static CONST char EXT[11] = ".wombat.il";
    
    IrFn flatten_function(Ptr<FnNode>& fn);
    LoweredBlock flatten_block(Ptr<BlockNode>& block);
    Ptr<Operand> flatten_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr);

    // Dev wants to create a `.wombat.il` file.
    bool dumpable() {
        return src.has_value();
    }

    fs::path transform_extension(const fs::path& src) {
        fs::path result = src;
        result.replace_extension(EXT);
        return result;
    };

OpCode ir_op_from_bin(const BinOpKind& op) {
    switch(op) {
        case BinOpKind::Add:        return OpCode::Add;
        case BinOpKind::Sub:        return OpCode::Sub;
        case BinOpKind::Mul:        return OpCode::Mul;
        case BinOpKind::Div:        return OpCode::Div;
        case BinOpKind::FlooredDiv: return OpCode::FlooredDiv;
        case BinOpKind::Mod:        return OpCode::Mod;
        case BinOpKind::And:        return OpCode::And;
        case BinOpKind::Or:         return OpCode::Or;
        case BinOpKind::BitXor:     return OpCode::BitXor;
        case BinOpKind::BitAnd:     return OpCode::BitAnd;
        case BinOpKind::BitOr:      return OpCode::BitOr;
        case BinOpKind::Shl:        return OpCode::Shl;
        case BinOpKind::Shr:        return OpCode::Shr;
        case BinOpKind::Eq:         return OpCode::Eq;
        case BinOpKind::Lt:         return OpCode::Lt;
        case BinOpKind::Le:         return OpCode::Le;
        case BinOpKind::NotEq:      return OpCode::NotEq;
        case BinOpKind::Ge:         return OpCode::Ge;
        case BinOpKind::Gt:         return OpCode::Gt;
        default: {
            ASSERT(false, std::format("[ir::err] unsupported binary op: '{}'", bin_op_str(op)));
            return OpCode::Nop;
        }
    }
}


    OpCode ir_op_from_un(const UnOpKind op) {
        switch(op) {
            case UnOpKind::Neg: return OpCode::Neg;
            case UnOpKind::BitNot: return OpCode::BitNot;
            case UnOpKind::Not: return OpCode::Not;
            default: {
                ASSERT(false, std::format("[ir::err] unsupported unary code: '{}'", un_op_str(op)));
                return OpCode::Nop;
            }
        }
    }

    inline Ptr<LitOp> new_lit_op(RawVal&& value, LiteralKind&& kind) {
        return mk_ptr(LitOp{ std::move(value), std::move(kind) });
    }

    inline Ptr<VarOp> new_var_op(RawVal&& name) {
        return mk_ptr(VarOp{ std::move(name) });
    }

    inline Ptr<TempOp> new_tmp_op(size_t&& id) {
        return mk_ptr(TempOp{ std::move(id) });
    }


    inline size_t push_temp() {
        return temp_counter++;
    }

    inline size_t push_loop() {
        return loop_counter++;
    }

    inline size_t push_branch() {
        return branch_counter++;
    }
};

#endif // IR_HPP_