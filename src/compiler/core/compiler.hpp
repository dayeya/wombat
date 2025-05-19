#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <filesystem>
#include "alias.hpp"
#include "builder.hpp"
#include "lex.hpp"
#include "ast.hpp"
#include "ir.hpp"
#include "gen.hpp"
#include "diag.hpp"

struct Context {
    LazyTokenStream program_tokens;
    AST program_ast;
    Ptr<IrProgram> ir_program;
    CodeGen backend;

    Context() : program_tokens(), program_ast(), ir_program(), backend() {}
    ~Context() = default;
};

class Compiler {
public:
    Context ctxt;
    // CodeGen backend;
    Diagnostics diagnostics;

    static CONST int MAX_DIAG_CAPACITY = 10;

    Compiler() : ctxt(), diagnostics(Compiler::MAX_DIAG_CAPACITY) {}
    
    void compile_target(const BuildConfig& build_config);
    
private:
    Verbosity verb;

    void lex(const BuildConfig& build_config);
    void parse(const BuildConfig& build_config);
    void sema_analyze(const BuildConfig& build_config);
    void lower_into_ir(const BuildConfig& build_config);
    void generate_asm_code(const BuildConfig& build_config);
    void add_diagnostic(const Diagnostic& diag);
    void execute(fs::path& exe);

    // Links the .asm file into an object file.
    fs::path build_target_into_exectuable(fs::path& asm_path, fs::path& out_exe_path);

    void log_if_debug(std::string&& info) {
        if(verb == Verbosity::Debug) {
            auto log = std::format("#[DEBUG] {}\n", std::move(info));
            std::printf("%s", log.c_str());
        }
    }

    void log_if_verbose(std::string&& info) {
        if(verb == Verbosity::Verbose) {
            auto log = std::format("#[LOG] {}\n", std::move(info));
            std::printf("%s", log.c_str());
        }
    }
};

#endif // COMPILER_HPP_
