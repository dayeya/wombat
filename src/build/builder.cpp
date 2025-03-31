#include "env.hpp"
#include "file.hpp"
#include "builder.hpp"

void Builder::clean() {
    // TODO("cleaning Builder.\n");
    return;
}

Option<std::string> Builder::next(std::span<char*>& args, size_t& k) {
    if(args[k + 1]) {
        return args[k + 1];
    }
    return std::nullopt;;
};

void Builder::parse_input_file(std::span<char*>& args, size_t& cur, std::string input) {
    ASSERT(args[cur], "Range exceeded when parsing an input file.");

    if(!real_loc(input)) {
        dump_err_and_exit(OpCode::Internal, std::string("cannot find ") + input);
    }
    if(is_directory(input)) {
        dump_err_and_exit(OpCode::Internal, "provide a valid source file");
    }
    if(!matches_ext(input, SRC_EXTENSTION)) {
        dump_err_and_exit(
            OpCode::Internal, 
            "invalid file format", 
            "try " + std::string(SRC_EXTENSTION) + " instead"
        );
    }

    // Set the target source.
    config.src = input;
}

void Builder::parse_out_file(std::span<char*>& args, size_t& cur) {
    auto out = next(args, cur);

    if(!out) {
        dump_err_and_exit(
            OpCode::NoInput, 
            "-o requires an input file", 
            "try --help for more detailed information"
        );
    }

    // Bump into the given file.
    ++cur;
    StrLoc obj = out.value();

    if(!real_loc(obj)) {
        dump_err_and_exit(OpCode::Internal, std::string("cannot find ") + obj);
    }
    if(is_directory(obj)) {
        dump_err_and_exit(OpCode::Internal, "provide a valid source file");
    }
    if(!matches_ext(obj, OUT_EXTENSION)) {
        dump_err_and_exit(
            OpCode::Internal, 
            "invalid file format", 
            "try " + std::string(OUT_EXTENSION) + " instead"
        );
    }
    
    // Set the target location.
    config.dst = obj;
}

void Builder::parse_option(std::span<char*>& args, size_t& cur, const std::string& opt) {
    ASSERT(cur < args.size(), "Range exceeded when parsing an optional.");
    if(opt == "-v0") {
        config.verb = Verbosity::Verbose; 
    } else if(opt == "-v1") { 
        config.verb = Verbosity::Debug; 
    } else if(opt == "-q") {
        config.verb = Verbosity::Quiet;
    } else if(opt == "-o") {
        parse_out_file(args, cur);
    } else if(opt == "-C") {
        config.compile_only = true;
    } else if(opt == "-S") {
        config.compile_and_assemble = true;
    } else if(opt == "-ast") {
        config.print_ast = true;
    } else if(opt == "-lx") {
        config.print_tokens = true;
    } else if(opt == "--version") {
        version();
        exit_builder(OpCode::Success);
    } else if(opt == "--help") {
        usage(config.name.c_str());
        exit_builder(OpCode::Success);
    } else {
        dump_err_and_exit(
            OpCode::InvalidArgument, 
            "invalid argument: " + opt,
            "try --help for detailed information"
        );
    }
}

void Builder::init(const std::string& parent_exec) {
    config = BuildConfig(
        parent_exec,
        std::nullopt,
        std::nullopt,
        false,
        false,
        false, 
        false,
        Verbosity::Normal
    );
}

void Builder::parse_arguments(int argc, char** argv) {
    if (argc < 2) {
        dump_err_and_exit(OpCode::NotEnoughArguments, "missing main target to compile");
        return;
    }

    size_t cur = 0;
    std::span<char*> args(argv, argc);

    while(args[cur])
    {   
        const auto& cur_arg = std::string(args[cur]);

        if(cur_arg.starts_with(OPTION_PREFIX)) {
            parse_option(args, cur, cur_arg);
        }
        else if(!config.src) {
            parse_input_file(args, cur, cur_arg);
        } else {
            dump_err_and_exit(OpCode::Internal, "invalid argument: " + cur_arg);
        }

        // Bump into the next.
        ++cur;
    }

    // Enforce the user to provide a target.
    if(!config.src) {
        dump_err_and_exit(OpCode::Internal, "missing main target to compile");
    }
}

void Builder::exit_builder(OpCode code) {
    clean();
    std::exit(stdlib_status_from_op_code(code));
}

void Builder::suggest(const std::string& suggestion) const {
    std::fprintf(stderr, "~ %s\n", suggestion.c_str());
}

void Builder::dump_err_and_exit(OpCode code, const std::string& description, const std::string& suggestion) {
    if(!description.empty()) {
        std::fprintf(stderr, "[%s::err] %s\n", COMPILER_NAME, description.c_str());
    }
    if(!suggestion.empty()) {
        suggest(suggestion);
    }
    exit_builder(code);
}

void Builder::usage(const char* exec) const {
    std::printf("Usage: %s [options] <FILE>", exec);
    std::printf("\n\n");
    std::printf("Options:\n");
    std::printf("    -o <FILE>      - Write an exectuable into <EXE_PATH>.\n");
    std::printf("                     Defaults to the ~/cwd/<file_name.wombat.out>.\n");
    std::printf("    --version      - Print version information.\n");
    std::printf("    --help         - Display this help menu.\n");
    std::printf("    -C             - Compile only, do not link or assemble.\n");
    std::printf("    -S             - Compile and assemble, do not link.\n");
    std::printf("    -q             - Disable detailed process logging.\n");
    std::printf("    -v0            - Enable detailed subprocess logging.\n");
    std::printf("    -v1            - Enable deeper subprocess logging.\n");
    std::printf("    -ast           - Add to the verbose output the parsed AST.\n");
    std::printf("    -lx            - Add to the verbose output the lexed tokens.\n");
    std::printf("\n");
}

void Builder::version() const {
    std::printf("%s - %s\n", COMPILER_NAME, VERSION_NUMBER);
}