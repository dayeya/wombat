#ifndef GEN_HPP_
#define GEN_HPP_

#include "asm.hpp"

class CodeGen {
public:
    // Name of wombat source file.
    StrLoc src;
    
    CodeGen() : src(), stack(), raw_program() {}
    CodeGen(StrLoc str) : src{std::move(str)}, stack(), raw_program() {}
    void assemble(IrProgram& program);

    fs::path transform_extension(fs::path&& from) {
        fs::path asm_file { from };
        asm_file.replace_extension(CodeGen::EXT);
        return asm_file;
    }

private:
    static CONST char EXT[5] = ".asm";

    // A function call stack.
    AsmStack stack;
    // Accumulator of generated assembly code.
    std::stringstream raw_program;
    // The current indentation level of asm code.
    size_t depth;

    void emit_header(IrProgram& ir);
    void emit_data_section(IrProgram& ir);
    void emit_text_section(IrProgram& ir);
    void emit_function(IrFn& func);
    void emit_instruction(Instruction& inst);
    
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

    void append_inline_comment(String&& line) { 
        raw_program << DOC << line << NEWLINE; 
    };

    void append(String&& line) { 
        raw_program << std::string(depth, TAB) << line << NEWLINE; 
    };

    void increase_depth() { 
        depth++; 
    };

    void decrease_depth() { 
        depth--; 
    };
};

#endif //  GEN_HPP_