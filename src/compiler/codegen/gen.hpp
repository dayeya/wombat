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

    size_t align_to(size_t n, size_t align) {
        return (n + align - 1) & ~(align - 1);
    }

    void append_inline_comment(String&& line) { 
        raw_program << DOC << line << NEWLINE; 
    };

    void append(String&& line) { 
        raw_program << std::string(depth, TAB) << line; 
    };

    void appendln(String&& line) {
        raw_program << std::string(depth, TAB) << line << NEWLINE;
    }

    void increase_depth() { depth++; };
    void decrease_depth() { depth--; };

    void log(String&& log) {
        std::printf("%s\n", log.c_str());
    }
};

#endif //  GEN_HPP_