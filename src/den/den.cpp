#include "den.hpp"

void Den::clean() {
    TODO("cleaning den.\n");
}

void Den::init(std::string parent_exec) {
    parse_time = ParseTime::None;
    den_options.init(parent_exec);
}

Option<std::string> Den::next(ArgViewer& args, size_t& k) {
    if(k + 1 < args.size()) {
        return args[k + 1];
    }
    return std::nullopt;;
};

void Den::parse_cmd_input(ArgViewer& args, size_t& cur) {
    switch (den_options.cmd)
    {
        case DenCommand::Run:
        case DenCommand::Build:
        {
            auto input = next(args, cur);
            if(!input.has_value()) {
                den_exit(DenCode::NoInput); 
                return;
            }
            ++cur;
            den_options.input = input.value();
            break;
        }
        default:
        {
            UNREACHABLE();
        }
    }
}

void Den::parse_cmd(ArgViewer& args, size_t& cur, std::string arg) {
    ASSERT(cur < args.size(), "Range exceeded when parsing a command.");
    if(arg == "build") {
        den_options.cmd = DenCommand::Build;
        parse_cmd_input(args, cur);
    } else if(arg == "run") {
        den_options.cmd = DenCommand::Run;
        parse_cmd_input(args, cur);
    } else if(arg == "help") {
        den_options.cmd = DenCommand::Help;
        den_usage(den_options.name.c_str(), den_options.verb);
        den_exit(DenCode::DenSuccess);
    } else {
        auto unreach = std::string("unreachable command: ") + arg;
        UNREACHABLE(unreach.c_str());
    }
}

void Den::parse_option(ArgViewer& args, size_t& cur, std::string opt) {
    ASSERT(cur < args.size(), "Range exceeded when parsing an optional.");
    if(opt.starts_with("-v")) {
        if(opt == "-v") { den_options.verb = VerbosityLevel::Verbose; }
        else if(opt == "-vv") { den_options.verb = VerbosityLevel::HighVerbose; }
        else if(opt == "-vvv") { den_options.verb = VerbosityLevel::Debug; }
        else {
            UNREACHABLE();
        }
    } else if(opt == "-q") {
        den_options.verb = VerbosityLevel::Quiet;
    } else if(opt == "-o") {
        auto out_loc = next(args, cur);
        if(!out_loc.has_value()) {
            den_exit(DenCode::NoInput); 
            return;
        }
        ++cur;
        den_options.out_location = out_loc.value();
    } else {
        UNREACHABLE();
    }
}

void Den::parse(int argc, char** argv) {

    if (argc < 2) {
        den_exit(DenCode::NotEnoughArguments);
        return;
    }

    size_t cur = 0;
    ArgViewer args(argv, argc);
    
    parse_time = ParseTime::None;
    for (cur = 1; cur < args.size(); ++cur) 
    {   
        const auto& cur_arg = std::string(args[cur]);

        if(den_options.cmd == DenCommand::Missing) {
            parse_time = ParseTime::Command;
            parse_cmd(args, cur, cur_arg);
        } else if(cur_arg.starts_with(OPTION_PREFIX)) {
            parse_time = ParseTime::Optional;
            parse_option(args, cur, cur_arg);
        } else {
            den_exit(DenCode::InvalidArgument);
        }
    }
}

void Den::den_suggest(const std::string& suggestion) {
    std::printf("~ %\n", suggestion.c_str());
}

void Den::den_show_error_diagnostic(const DenCode& code) {
    switch (den_options.cmd)
    {
        case DenCommand::Run: 
        case DenCommand::Build:
        {
            if (parse_time == ParseTime::Command) {
                std::printf("den_error: the %s command requires input files.\n", den_options.command_meaning().c_str());
            } else {
                std::printf("den_error: the ‘-o’ command requires input files.\n");
            }
            break;
        }
        case DenCommand::Missing:
        {
            // A command was yet to be parsed.
            if(parse_time == ParseTime::Optional) {
                // An error occured during an <option> parsing.
                switch (code)
                {
                    case DenCode::NoInput:
                    {
                        // This error can only occur during parsing the `-o` option.
                        std::printf("den_error: the ‘-o’ command requires input files.\n");
                        return;
                    }
                    default: 
                        // Ignore any codes since all other errors occur during command parsing.
                        break;
                }
            } else {
                UNREACHABLE();
            }
            break;
        }
        default:
        {
            UNREACHABLE();
        }
    }
}

void Den::den_exit(const DenCode& code) {
    int exit_code = -1;
    switch (code)
    {
        case DenCode::DenSuccess: {
            std::printf("[DEN::LOG] successful den session.\n");
            exit_code = EXIT_SUCCESS;
            break;
        }
        case DenCode::NoInput: {
            den_show_error_diagnostic(code);
            exit_code = EXIT_FAILURE;
            break;
        }
        case DenCode::NotEnoughArguments: {
            den_usage(den_options.name.c_str(), den_options.verb);
            exit_code = EXIT_FAILURE;
            break;
        }
        default:
        {
            UNREACHABLE();
        }
    }
    clean();
    std::exit(exit_code);
}

void Den::den_usage(const char* exec, VerbosityLevel verb) const {
    std::printf("Usage: %s [OPTIONS] <command> [INPUTS]", exec);
    std::printf("\n\n");
    std::printf("Commands:\n");
    std::printf("    build [SOURCE_PATH] <FLAGS> - Compile the source file at <SOURCE_PATH> into an executable.\n");
    std::printf("    run [SOURCE_PATH] <FLAGS>   - Compile and run the executable.\n");
    std::printf("    help                        - Display this help menu.\n");
    std::printf("\n\n");
    std::printf("Options:\n");
    std::printf("    -ver                        - Print version information.\n");
    std::printf("    -v,                         - Enable detailed process logging, in default will log data\n");
    std::printf("    -s,                         - Disable detailed process logging.\n");
    std::printf("    -o [EXE_PATH]               - Write an exectuable into <EXE_PATH>.\n");
    std::printf("\n");
}