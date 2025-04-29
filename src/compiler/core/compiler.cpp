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

    TODO("Dump any diagnostics from the lexer into the compiler.");
}

void Compiler::parse(const BuildConfig& build_config) {
    ASSERT(ctxt.program_tokens.has_next(), "cannot parse an empty token stream.");

    Parser parser(ctxt.program_tokens);
    parser.parse(ctxt.program_ast);

    if(build_config.print_ast) {
        PPVisitor pp_visitor(std::cout);
        ctxt.program_ast.traverse(pp_visitor);
    }

    TODO("Dump any diagnostics from the parser into the compiler.");
}

void Compiler::compile_target(const BuildConfig& build_config) {
    ASSERT(build_config.src.has_value(), "missing target to compile.");
    
    lex(build_config);
    parse(build_config);
}

void Compiler::build_target_into_exectuable() {
    ASSERT(false, "`Compiler::build_target_into_exectuable` is not implemented.");
}
