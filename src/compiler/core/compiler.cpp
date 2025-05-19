#include <cstdlib>
#include <string>
#include <filesystem>
#include <format>

#include "env.hpp"
#include "compiler.hpp"

void Compiler::lex(const BuildConfig& config) {
    ASSERT(config.src.has_value(), "File validation bypassed — source path is missing.");

    Lexer lexer(config.src.value());
    ctxt.program_tokens = lexer.lex_source();

    if (config.print_tokens) {
        ctxt.program_tokens.dump();
    }

    log_if_debug("Lexer completed. Diagnostic tokens captured if any.");
}

void Compiler::parse(const BuildConfig& config) {
    ASSERT(ctxt.program_tokens.has_next(), "Cannot parse an empty token stream.");

    Parser parser(ctxt.program_tokens);
    parser.parse(ctxt.program_ast);

    if (config.print_ast) {
        PPVisitor pp_visitor(std::cout);
        ctxt.program_ast.traverse(pp_visitor);
    }

    log_if_debug("Parser completed. AST ready.");
}

void Compiler::sema_analyze(const BuildConfig& config) {
    ASSERT(!ctxt.program_ast.functions.empty(), "Semantic analysis attempted on empty AST.");

    SemanticVisitor sema;
    sema.include_builtins();

    for (auto& fn : ctxt.program_ast.functions) {
        fn->analyze(sema);
    }

    log_if_debug("Semantic analysis completed.");
}

void Compiler::lower_into_ir(const BuildConfig& config) {
    auto ir = std::make_unique<IrProgram>();
    ir->gen(ctxt.program_ast);

    if (config.print_ir) {
        ir->src = config.src.value();
        ir->dump();
    }

    ctxt.ir_program = std::move(ir);
    log_if_debug("Lowered into intermediate representation.");
}

void Compiler::generate_asm_code(const BuildConfig& config) {
    CodeGen generator;
    generator.assemble(*ctxt.ir_program);
    ctxt.backend = std::move(generator);

    log_if_debug("Assembly code generation completed.");
}

void Compiler::compile_target(const BuildConfig& config) {
    ASSERT(config.src.has_value(), "No source file specified to compile.");

    verb = config.verb;

    lex(config);
    parse(config);
    sema_analyze(config);
    lower_into_ir(config);
    generate_asm_code(config);

    build_project(config);
}

Path Compiler::build_bin_dir(const BuildConfig& config) {
    Path sourcePath(config.src.value());
    Path binPath = sourcePath.parent_path() / BIN;
    fs::create_directories(binPath);
    return binPath;
}

std::pair<Path, std::ofstream> Compiler::create_asm_file(const Path& bin_dir, const String& src) {
    Path src_path(src);
    Path asmPath = bin_dir / src_path.filename();
    asmPath.replace_extension(CodeGen::EXT);

    std::ofstream asm_file;
    return { asmPath, std::move(asm_file) };
}

void Compiler::build_project(const BuildConfig& config) {
    Path bin_dir = build_bin_dir(config);
    auto [asm_file_path, asm_file] = create_asm_file(bin_dir, config.src.value());

    asm_file.open(asm_file_path);
    ASSERT(asm_file.is_open(), "Failed to open assembly file for writing.");

    asm_file << ctxt.backend.get_raw_program();
    asm_file.close();

    Path out_path = asm_file_path;
    out_path.replace_extension(OUT_EXTENSION);

    Path std_lib_path = fs::canonical(Path(__FILE__).parent_path().parent_path().parent_path() / "std" / "std.asm");
    Path std_object_file = build_std_lib(std_lib_path);
    Path executable_path = build_target_into_executable(std_object_file, asm_file_path, out_path);

    if (config.run) {
        execute(executable_path);
    }
}

Path Compiler::build_std_lib(const Path& std_path) {
    Path object_file = std_path;
    object_file.replace_extension(".o");

    String cmd = format("nasm -f elf64 -o {} {}", object_file.string(), std_path.string());
    log_if_verbose(format("Building stdlib: {}", cmd));
    ASSERT(system(cmd.c_str()) == 0, format("[linker::err] Failed to assemble standard library: {}", cmd));

    return object_file;
}

Path Compiler::build_target_into_executable(
    const Path& std_object_file, 
    const Path& asm_file_path, 
    const Path& out_path
) {
    Path objFile = asm_file_path;
    objFile.replace_extension(".o");

    String asmCmd = format("nasm -f elf64 -o {} {}", objFile.string(), asm_file_path.string());
    log_if_verbose(format("Assembling program: {}", asmCmd));
    ASSERT(system(asmCmd.c_str()) == 0, format("[linker::err] Failed to assemble target: {}", asmCmd));

    String linkCmd = format("ld -o {} {} {}", out_path.string(), objFile.string(), std_object_file.string());
    log_if_verbose(format("Linking executable: {}", linkCmd));
    ASSERT(system(linkCmd.c_str()) == 0, format("[linker::err] Linking failed: {}", linkCmd));

    fs::remove(objFile);
    return out_path;
}

void Compiler::execute(Path& executablePath) {
    permissions(executablePath,
        fs::perms::owner_exec |
        fs::perms::group_exec |
        fs::perms::others_exec,
        fs::perm_options::add
    );

    log_if_verbose(format("[INFO] Executing: {}", executablePath.string()));

    int result = system(executablePath.c_str());
    ASSERT(result != -1, "Execution failed to start.");

    if (WIFEXITED(result)) {
        int exitStatus = WEXITSTATUS(result);
        ASSERT(exitStatus == 0, format("[WARN] Program exited with non-zero status: {}", exitStatus));
        log_if_verbose("Program completed successfully.");
    } else {
        ASSERT(false, "Program terminated unexpectedly.");
    }
}