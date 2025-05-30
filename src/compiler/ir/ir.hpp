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

    // File name for wombat.il file generation.
    Option<StrLoc> src;
    // Total function bodies.
    LoweredProgram lowered_program;
    // How many stack bytes does the function need?.
    // Resets to 0 after every function.
    size_t cur_frame_size;

    IrProgram() : src{std::nullopt}, lowered_program{}, cur_frame_size{0} {}
    IrProgram(StrLoc name) 
        : src{name}, 
          lowered_program{},
          loop_stack(), 
          cur_frame_size{0},
          branch_counter{0},
          temp_counter{0} {}

    // Writes a textual structure of the 'IR' into a file.
    void dump();

    // Generate an 'IR' from an ast.
    void gen(AST& ast);

private:
    struct LoopCtx {
        String brk;
        String cnt;
    };
    using LoopStack = std::vector<LoopCtx>;

    LoopStack loop_stack;
    size_t temp_counter = 0;
    size_t branch_counter = 0;
    
    static CONST int TEMP_SIZE = 8;
    static CONST char EXT[11] = ".wombat.il";
    
    // Compound types.
    IrFn flatten_function(Ptr<FnNode>& fn);
    LoweredBlock flatten_block(Ptr<BlockNode>& ctx);

    // expression flattening.
    Ptr<Operand> flatten_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr);
    Ptr<Operand> flatten_expr_into_addr(LoweredBlock& ctx, Ptr<ExprNode>& expr);
    Ptr<Operand> flatten_lit_expr(Ptr<ExprNode>& expr);
    Ptr<Operand> flatten_bin_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr);
    Ptr<Operand> flatten_un_expr(LoweredBlock& ctx, Ptr<ExprNode>& expr);
    Ptr<Operand> flatten_terminal(LoweredBlock& ctx, Ptr<ExprNode>& expr);
    Ptr<Operand> flatten_fn_call_from_expr(LoweredBlock& ctx, Ptr<ExprNode>& fn_call);

    // statement flattening.
    void flatten_fn_call_from_stmt(LoweredBlock& ctx, Ptr<StmtNode>& fn_call);
    void flatten_var_decl(LoweredBlock& ctx, Ptr<StmtNode>& var_decl);
    void flatten_assign(LoweredBlock& ctx, Ptr<StmtNode>& assign);
    void flatten_deref_assign(LoweredBlock& ctx, Ptr<StmtNode>& deref);
    void flatten_ret_stmt(LoweredBlock& ctx, Ptr<StmtNode>& ret_stmt);
    void flatten_loop_stmt(LoweredBlock& ctx, Ptr<StmtNode>& loop_stmt);
    void flatten_brk_stmt(LoweredBlock& ctx, Ptr<StmtNode>& break_stmt);
    void flatten_branch(LoweredBlock& ctx, Ptr<StmtNode>& if_stmt);
    void flatten_only_if(LoweredBlock& ctx, Ptr<Operand>& op, Ptr<BlockNode>& if_block);
    void flatten_if_and_else(LoweredBlock& ctx, Ptr<Operand>& op, Ptr<BlockNode>& if_block, Ptr<BlockNode>& else_block);

    // Dev wants to create a `.wombat.il` file.
    bool dumpable() {
        return src.has_value(); 
    }

    fs::path transform_extension(const fs::path& from) {
        fs::path result = from;
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
            case UnOpKind::Dereference: return OpCode::Dereference;
            default: {
                ASSERT(false, std::format("[ir::err] unsupported unary code: '{}'", un_op_str(op)));
                return OpCode::Nop;
            }
        }
    }

    inline String gen_branch_label(String&& ty) {
        return std::format(".br_{}{}", ty, branch_counter);
    }

    inline Ptr<LitOp> new_lit_op(String&& value, LiteralKind&& kind) {
        return mk_ptr(LitOp{ std::move(value), std::move(kind) });
    }

    inline Ptr<VarOp> new_var_op(String&& name) {
        return mk_ptr(VarOp{ std::move(name) });
    }

    inline Ptr<TempOp> new_tmp_op(size_t&& id) {
        return mk_ptr(TempOp{ std::move(id) });
    }

    inline Ptr<LabelOp> new_lbl_op(String ident) {
        return mk_ptr(LabelOp{ std::move(ident) });
    }

    inline Ptr<AddrOp> new_addr_op(String&& addr) {
        return mk_ptr(AddrOp{ std::move(addr) });
    }

    inline void push_loop() {
        push_branch();
        loop_stack.push_back(LoopCtx{gen_branch_label("loop_brk"), gen_branch_label("loop_cnt")});
    }

    inline void pop_loop() {
        loop_stack.pop_back();
    }

    inline size_t push_temp() {
        return temp_counter++;
    }

    inline size_t push_branch() {
        return branch_counter++;
    }
};

#endif // IR_HPP_