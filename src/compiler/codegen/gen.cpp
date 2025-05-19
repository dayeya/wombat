#include <fstream>
#include "gen.hpp"

namespace fs = std::filesystem;

void CodeGen::emit_header(IrProgram&) {
    append_inline_comment("global entry point");
    append("global _start");
    append_inline_comment("external linkage");
    append("extern write");
    append("extern exit");
    append("");
}

void CodeGen::emit_data_section(IrProgram&) {
    append("section .data");
    increase_depth();
    append("hello_msg db \"Hello, world!\", 10");  // 10 is newline
    append("hello_len equ $ - hello_msg");
    decrease_depth();
    append("");
}

void CodeGen::emit_text_section(IrProgram&) {
    append("section .text");
    append("_start:");
    increase_depth();

    append_inline_comment("syscall write(stdout, hello_msg, hello_len)");
    append("mov rax, 1");              // syscall: write
    append("mov rdi, 1");              // fd: stdout
    append("mov rsi, hello_msg");      // pointer to message
    append("mov rdx, hello_len");      // message length
    append("syscall");

    append("");

    append_inline_comment("syscall exit(0)");
    append("mov rax, 60");             // syscall: exit
    append("mov rdi, 0");              // exit code
    append("syscall");

    decrease_depth();
    append("");
}

void CodeGen::assemble(IrProgram& program) {
    // Reset state
    raw_program.str("");
    raw_program.clear();
    depth = 0;

    emit_header(program);
    emit_data_section(program);
    emit_text_section(program);

    // Dump output
    fs::path out_path = transform_extension(fs::path(src));
    std::ofstream out_file(out_path);

    ASSERT(out_file.is_open(), format("[codegen::err] Could not open output file: {}", out_path.string()));
    out_file << raw_program.str();
}