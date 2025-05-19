#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <filesystem>
#include <fstream>
#include "alias.hpp"
#include "builder.hpp"
#include "lex.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "ir.hpp"
#include "file.hpp"
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
    
    void compile_target(const BuildConfig& config);
    
private:
    Verbosity verb;

    static CONST char BIN[11] = "wombat_bin";

    void build_project(const BuildConfig& config);
    void lex(const BuildConfig& config);
    void parse(const BuildConfig& config);
    void sema_analyze(const BuildConfig& config);
    void lower_into_ir(const BuildConfig& config);
    void generate_asm_code(const BuildConfig& config);
    void add_diagnostic(const Diagnostic& diag);
    void execute(Path& exe);

    // Links the .asm file into an object file.
    Path build_std_lib(const Path& std_path);
    Path build_bin_dir(const BuildConfig& config);
    Path build_target_into_executable(const Path& std_object_file, const Path& asm_path, const Path& out_exe_path);
    std::pair<Path, std::ofstream> create_asm_file(const Path& bin_dir, const String& src);

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
