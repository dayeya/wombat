#ifndef GEN_HPP_
#define GEN_HPP_

#include <string>
#include "asm.hpp"

class CodeGen {
public:
    static CONST char EXT[5] = ".asm";
    static CONST char STD[15] = "../std/std.asm";
    
    CodeGen() 
        : stack(), 
          raw_program(), 
          register_map(), 
          abi_registers(), 
          depth{0},
          argument_position{0} {}

    void assemble(IrProgram& program);

    inline std::string get_raw_program() const { 
        return raw_program.str(); 
    }    

    fs::path transform_extension(fs::path& from) {
        fs::path asm_file { from };
        asm_file.replace_extension(CodeGen::EXT);
        return asm_file;
    }

private:
    using _Regs = std::vector<Register>;
    using _RegMap = std::unordered_map<String, bool>;

    AsmStack stack;
    _RegMap register_map;
    _Regs abi_registers;
    size_t depth;
    size_t argument_position;
    std::stringstream raw_program;
    std::stringstream conv;

    // Size of temporary variables in memory.
    static CONST int TEMP_SIZE = 8;

    void set_abi_registers();
    void emit_header(IrProgram& ir);
    void emit_data_section(IrProgram& ir);
    void emit_text_section(IrProgram& ir);

    void emit_function(IrFn& func);
    void emit_instruction(IrFn& fn, Instruction& inst);
    void emit_call(Instruction& inst);
    void emit_assign(Instruction& inst);
    void emit_alloc(Instruction& inst);
    void emit_push(Instruction& inst);
    void emit_pop(Instruction& inst);
    void emit_ret(Instruction& inst);
    void emit_add(Instruction& inst);
    void emit_sub(Instruction& inst);
    void emit_mul(Instruction& inst);
    void emit_div(Instruction& inst);
    void emit_mod(Instruction& inst);
    void emit_neg(Instruction& inst);
    void emit_bitnot(Instruction& inst);
    void emit_bitand(Instruction& inst);
    void emit_bitxor(Instruction& inst);
    void emit_bitor(Instruction& inst);
    void emit_logical_not(Instruction& inst);
    void emit_logical_and(Instruction& inst);
    void emit_logical_or(Instruction& inst);
    void emit_label(Instruction& inst);
    void emit_jmp(Instruction& inst);
    void emit_jmp_false(Instruction& inst);
    void emit_cmp(Instruction& inst);

    // Loads into the given registers memory 'op'.
    void load_operand(Ptr<Operand>& op, String&& reg, Option<String> sym);

    // Returns the current available register for pipelining function arguments.
    Option<Register> register_for_arguement_pipelining();

    // Cleans all the registers until 'passed_arguments', if 'passed_arguments' is more than 6, we clear the stack.
    void clean_registers(size_t passed_arguments);

    // Is the operand a symbol? for register allocation.
    Option<String> gain_symbol(Ptr<Operand>& op) {
        if(op->kind == OpKind::Lit) {
            return std::nullopt;
        }
        return op->as_str();
    }

    String reg_to_str(Register reg) {
        switch (reg) {
            case Register::Rax: return "rax";
            case Register::Rbx: return "rbx";
            case Register::Rdi: return "rdi";
            case Register::Rsi: return "rsi";
            case Register::Rdx: return "rdx";
            case Register::Rcx: return "rcx";
            case Register::R8:  return "r8";
            case Register::R9:  return "r9";
            default:
                ASSERT(false, "unreachable");
        }
    }

    inline void occupy_register(Register& reg) {
        register_map[reg_to_str(reg)] = true;
    }

    inline void clean_register(Register& reg) {
        register_map[reg_to_str(reg)] = false;
    }

    inline size_t align_to(size_t n, size_t align) {
        return (n + align - 1) & ~(align - 1);
    }

    std::string mem_ident_from_size(size_t size) {
        switch(size) {
            case 8: return "qword"; 
            case 4: return "dword"; 
            case 2: return "word"; 
            case 1: return "byte";
            default: {
                ASSERT(false, format("[codegen::err] invalid operand size, {}", size));
                return "";
            } 
        }
    }

    String register_variant_from_size(Register reg, size_t memsize) {
        switch (reg) {
            case Register::Rdi:
                switch (memsize) {
                    case 1: return "dil";
                    case 2: return "di";
                    case 4: return "edi";
                    case 8: return "rdi";
                }
                break;
            case Register::Rsi:
                switch (memsize) {
                    case 1: return "sil";
                    case 2: return "si";
                    case 4: return "esi";
                    case 8: return "rsi";
                }
                break;
            case Register::Rdx:
                switch (memsize) {
                    case 1: return "dl";
                    case 2: return "dx";
                    case 4: return "edx";
                    case 8: return "rdx";
                }
                break;
            case Register::Rcx:
                switch (memsize) {
                    case 1: return "cl";
                    case 2: return "cx";
                    case 4: return "ecx";
                    case 8: return "rcx";
                }
                break;
            case Register::R8:
                switch (memsize) {
                    case 1: return "r8b";
                    case 2: return "r8w";
                    case 4: return "r8d";
                    case 8: return "r8";
                }
                break;
            case Register::R9:
                switch (memsize) {
                    case 1: return "r9b";
                    case 2: return "r9w";
                    case 4: return "r9d";
                    case 8: return "r9";
                }
                break;
            default:
                UNREACHABLE();
        }
    }

    void append_inline_comment(String&& line) { 
        raw_program << DOC << line << NEWLINE; 
    };

    void append(String&& line) { 
        raw_program << std::string(depth, TAB) << line; 
    };

    void appendln(String&& line) {
        ASSERT(depth >= 0, "depth must be greater or equal to 0");
        raw_program << std::string(depth, TAB) << line << NEWLINE;
    }

    void increase_depth(int factor = 1) { 
        depth += factor; 
    };

    void decrease_depth(int factor = 1) { 
        depth -= factor;
        if(depth < 0) {
            depth = 0;
        } 
    };

    void log(String&& log) {
        std::printf("%s\n", log.c_str());
    }
};

#endif //  GEN_HPP_