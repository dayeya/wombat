#include "env.hpp"
#include "compiler.hpp"

#include "lex.hpp"
#include "parser.hpp"

void Compiler::lex(const BuildConfig& build_config) {
    ASSERT(build_config.src.has_value(), "unreachable call stack, bypassed file validation.");

    Lexer lexer(build_config.src.value());
    ctxt.program_tokens = lexer.lex_source();

    if(build_config.print_tokens) {
        ctxt.program_tokens.dump();
    }

    log_if_debug("Dump any diagnostics from the lexer into the compiler.");
}

void Compiler::parse(const BuildConfig& build_config) {
    ASSERT(ctxt.program_tokens.has_next(), "cannot parse an empty token stream.");

    Parser parser(ctxt.program_tokens);
    parser.parse(ctxt.program_ast);

    if(build_config.print_ast) {
        PPVisitor pp_visitor(std::cout);
        ctxt.program_ast.traverse(pp_visitor);
    }

    log_if_debug("Dump any diagnostics from the parser into the compiler.");
}

void Compiler::sema_analyze(const BuildConfig& build_config) {
    ASSERT(!ctxt.program_ast.functions.empty(), "cannot analyze an empty AST.");

    SemanticVisitor sema_visitor;
    sema_visitor.include_builtins();
    
    for(auto& fn : ctxt.program_ast.functions) {
        fn->analyze(sema_visitor);
    }

    log_if_debug("Dump any diagnostics from the semantic analysis into the compiler.");
}

void Compiler::lower_into_ir(const BuildConfig& build_config) {
    IrProgram ir{};
    ir.gen(ctxt.program_ast);

    if(build_config.print_ir) {
        ir.src = build_config.src.value();
        ir.dump();
    }
}

void Compiler::compile_target(const BuildConfig& build_config) {
    ASSERT(build_config.src.has_value(), "missing target to compile.");
    
    // Set logging level.
    verb = build_config.verb;

    lex(build_config);
    parse(build_config);
    sema_analyze(build_config);
    lower_into_ir(build_config);
}

void Compiler::build_target_into_exectuable() {
    ASSERT(false, "`Compiler::build_target_into_exectuable` is not implemented.");
}