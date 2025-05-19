#include <cstdlib>
#include <string>
#include <filesystem>
#include <format>

#include "env.hpp"
#include "file.hpp"
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
    Ptr<IrProgram> ir = std::make_unique<IrProgram>();
    ir->gen(ctxt.program_ast);

    if(build_config.print_ir) {
        ir->src = build_config.src.value();
        ir->dump();
    }

    ctxt.ir_program = std::move(ir);
}
 
void Compiler::generate_asm_code(const BuildConfig& build_config) {
    CodeGen gen { build_config.src.value() };
    gen.assemble(*ctxt.ir_program);
    ctxt.backend = std::move(gen);

}

void Compiler::compile_target(const BuildConfig& build_config) {
    ASSERT(build_config.src.has_value(), "missing target to compile.");
    
    // Set logging level.
    verb = build_config.verb;

    lex(build_config);
    parse(build_config);
    sema_analyze(build_config);
    lower_into_ir(build_config);
    generate_asm_code(build_config);

    fs::path asm_path = ctxt.backend.transform_extension(ctxt.backend.src);
    fs::path out_path(ctxt.backend.src);
    out_path.replace_extension(OUT_EXTENSION);

    fs::path exe = build_target_into_exectuable(asm_path, out_path);
    
    if(build_config.run) {
        execute(exe);
    }
}

fs::path Compiler::build_target_into_exectuable(fs::path& asm_path, fs::path& out_exe_path) {
    fs::path obj_file = asm_path;
    obj_file.replace_extension(".o");

    // Assemble with NASM
    String asm_cmd = format("nasm -f elf64 -o {} {}", obj_file.string(), asm_path.string());
    log_if_verbose(format("Executing '{}'", asm_cmd));
    ASSERT(system(asm_cmd.c_str()) == 0, format("[linker::err] assembly process failed: {}", asm_cmd));

    String lnk_cmd = format("ld -o {} {}", out_exe_path.string(), obj_file.string());
    log_if_verbose(format("Executing '{}'", lnk_cmd));
    ASSERT(system(lnk_cmd.c_str()) == 0, format("[linker::err] linking failed: {}", lnk_cmd));

    fs::remove(obj_file);
    return out_exe_path;
}

void Compiler::execute(fs::path& exe) {
    // Making sure exe has valid permissions.
    permissions(exe,
        fs::perms::owner_exec | 
        fs::perms::group_exec | 
        fs::perms::others_exec, 
        fs::perm_options::add
    );

    log_if_verbose(format("[INFO] Executing: {}\n", exe.string()));

    int code = system(exe.c_str());
    ASSERT(code != -1, "failed to launch executable");

    int status = WEXITSTATUS(code);
    if (WIFEXITED(code)) {
        ASSERT(status == 0, format("[WARN] program exited with code {}", status));
        log_if_verbose("program completed successfully");
    } else {
        ASSERT(false, "wombat program terminated unexpectedly");
    }
}