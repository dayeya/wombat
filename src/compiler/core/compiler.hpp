#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <filesystem>
#include "alias.hpp"
#include "builder.hpp"
#include "lex.hpp"
#include "ast.hpp"
#include "ir.hpp"
#include "diag.hpp"

namespace fs = std::filesystem;

struct Context {
    LazyTokenStream program_tokens;
    AST program_ast;
    Ptr<IrProgram> ir_program;

    Context() : program_tokens(), program_ast(), ir_program() {}
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

    void finish();
    void build_target_into_exectuable();
    void add_diagnostic(const Diagnostic& diag);

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
