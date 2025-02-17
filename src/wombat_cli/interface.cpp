#include <iomanip>
#include <string>
#include <expected>
#include <queue>
#include <functional>

#include "args.hpp"
#include "emitter.hpp"
#include "callback.hpp"
#include "io_file.hpp"
#include "compiler.hpp"
#include "interface.hpp"

auto WInterface::build_interface(int argc, char** argv) -> WInterface {
    auto wombat_emitter = Emitter();
    std::queue<Callback> wombat_callbacks;

    // Lambda to register argument-parsing errors as callbacks.
    auto register_argument_diagnostic = [&](const Diagnostic& diag_from_arg_parsing) -> std::expected<Args, Diagnostic> {
        wombat_callbacks.emplace(
            CallbackIdentifer::ArgumentDiagnostic, 
            std::bind_front(&Emitter::emit_diagnostic, &wombat_emitter),
            diag_from_arg_parsing
        );
        return Args{};
    };

    auto register_input_validation_diagnostic = [&](const Diagnostic& diag_from_io_validation) -> std::expected<std::monostate, Diagnostic> {
        wombat_callbacks.emplace(
            CallbackIdentifer::IoFileValidation, 
            std::bind_front(&Emitter::emit_diagnostic, &wombat_emitter),
            diag_from_io_validation
        );
        return std::monostate{};
    };

    // Attempt to parse command-line arguments.
    // If parsing fails, recieve the diagnostic and register it.
    auto parsed_arguments = Args::parse_args(argc, argv)
        .or_else(register_argument_diagnostic)
        .value();

    //! Empty-in-file by default.
    InputFile input_file;
    if(parsed_arguments.has_option("--build") || parsed_arguments.has_option("-b")) {
        auto b_opt = parsed_arguments.get_option_value("-b");
        auto build_opt = parsed_arguments.get_option_value("--build");
    
        input_file.file = build_opt.value_or(b_opt.value_or(Option{ "undifined_option" })).value;
    
        input_file.validate_input_file()
            .or_else(register_input_validation_diagnostic)
            .value();
    }
    if(parsed_arguments.has_option("--run") || parsed_arguments.has_option("-r")) {
        auto r_opt = parsed_arguments.get_option_value("-r");
        auto run_opt = parsed_arguments.get_option_value("--run");
    
        input_file.file = run_opt.value_or(r_opt.value_or(Option{ "undifined_option" })).value;
    
        input_file.validate_input_file()
            .or_else(register_input_validation_diagnostic)
            .value();
    }

    OutputFile output_file;
    if(parsed_arguments.has_option("--outputfile") || parsed_arguments.has_option("-o")) {
        auto u_opt = parsed_arguments.get_option_value("-o");
        auto out_opt = parsed_arguments.get_option_value("--outputfile");
    
        output_file.file = out_opt.value_or(u_opt.value_or(Option{ "undifined_option" })).value;
    
        output_file.validate_input_file()
            .or_else(register_input_validation_diagnostic)
            .value();
    }

    WInterface wombat_interface(
        parsed_arguments,
        wombat_emitter,
        wombat_callbacks,
        input_file,
        output_file
    );

    return wombat_interface;
}

auto WInterface::execute() -> void {
    
    //! Flush early callbacks
    if(!callback_queue_.empty()) {
        flush_callbacks();
        return;
    }

    if(args_.has_flag("--help") || args_.has_flag("-h")) {
        help_information();
        return;
    }

    //! Handle --outdir option
    //! 
    //! if(args_.has_flag("--outdir") || args_.has_flag("-u")) {}
    //!

    //! Handle --build option
    //! 
    //! if(args_.has_flag("--build") || args_.has_flag("-r")) {}
    //!

    

    auto comp = Compiler();
    
    comp.start_lexing(
        args_.get_option_value("--build").value().value
    );
}

auto WInterface::flush_callbacks() -> void {
    while(!callback_queue_.empty()) {
        callback_queue_
            .front()
            .invoke();
        callback_queue_.pop();
    }
}

auto WInterface::help_information() -> void {
    std::cout << R"(Usage: wombat [OPTIONS] [INPUTS]

Options:
    -b, --build [SOURCE_PATH] <FLAGS>   Compile the source file at SOURCE_PATH.
                                        Use FLAGS to customize compilation.

    -r, --run [SOURCE_PATH] <FLAGS>     Compile and run the executable.
                                        Use FLAGS to customize compilation.

    -u, --outdir [DIR_PATH]             Specify the output directory. 
                                        Defaults to the current directory if omitted.

    -h, --help                          Display this help menu.

Flags:
    -v, --verbose                       Enable detailed process logging.

    -o, --out [EXE_PATH]                Specify the output executable path.
                                        Defaults to the same directory as the source.
)";
}
