#ifndef GEN_HPP_
#define GEN_HPP_

#include "asm.hpp"

class CodeGen {
public:
    static CONST char EXT[5] = ".asm";
    static CONST char STD[15] = "../std/std.asm";
    
    CodeGen() : stack(), raw_program(), registers(), depth(0) {}
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
    // A function call stack.
    AsmStack stack;
    // Accumulator of generated assembly code.
    std::stringstream raw_program;
    // A buffer for convertion purposes.
    std::stringstream conv;
    // The current indentation level of asm code.
    size_t depth;

    // Which register is already occupied? 
    std::unordered_map<String, bool> registers;

    void emit_header(IrProgram& ir);
    void emit_data_section(IrProgram& ir);
    void emit_text_section(IrProgram& ir);
    void emit_function(IrFn& func);
    void emit_instruction(IrFn& fn, Instruction& inst);
    
    // Loads into the given registers memory 'op'.
    void load_op_into_reg(const Ptr<Operand>& op, const String& owner, const Register& reg);

    // Returns the current available register for pipelining function arguments.
    Option<Register> register_for_arguement_pipelining();

    Register str_to_reg(String&& reg) {
        if (reg.compare("rax") == 0) return Register::Rax;
        if (reg.compare("rbx") == 0) return Register::Rbx;
        if (reg.compare("rdi") == 0) return Register::Rdi;
        if (reg.compare("rsi") == 0) return Register::Rsi;
        if (reg.compare("rdx") == 0) return Register::Rdx;
        if (reg.compare("rcx") == 0) return Register::Rcx;
        if (reg.compare("r8") == 0)  return Register::R8;
        if (reg.compare("r9") == 0)  return Register::R9;
        ASSERT(false, format("{} is not a valid register", reg));
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

    inline void occupy_register(Register&& reg) {
        registers[reg_to_str(std::move(reg))] = true;
    }

    inline void clean_register(Register&& reg) {
        registers[reg_to_str(std::move(reg))] = false;
    }

    size_t size_from_string(String&& size) {
        conv.str(size);
        size_t converted;
        conv >> converted;
        conv.clear();
        return std::move(converted);
    }

    std::string size_identifier_from_bytes(size_t size) {
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