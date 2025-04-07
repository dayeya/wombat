#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <filesystem>
#include "alias.hpp"
#include "builder.hpp"
#include "ctxt.hpp"
#include "diag.hpp"

namespace fs = std::filesystem;

class Compiler {
public:
    Context ctxt;
    // CodeGen backend;
    Diagnostics diagnostics;

    static CONST int MAX_DIAG_CAPACITY = 10;

    Compiler() 
        : ctxt(), diagnostics(Compiler::MAX_DIAG_CAPACITY) {}
    
    void compile_target(const BuildConfig& build_config);
    
private:
    void lex(const BuildConfig& build_config);
    void parse(const BuildConfig& build_config);
    
    bool caught_diags();
    bool caught_early_diags();

    void finish();
    void build_target_into_exectuable();
    void add_diagnostic(const Diagnostic& diag);
};

#endif // COMPILER_HPP_
