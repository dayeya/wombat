#include <iomanip>
#include <string>
#include <expected>
#include <queue>
#include <functional>

#include "args.hpp"
#include "diagnostic.hpp"
#include "callback.hpp"
#include "io_file.hpp"
#include "lex.hpp"
#include "interface.hpp"
#include "session.hpp"

void Interface::run(int argc, char** argv) {
    //! Args::parse_args function validates the user-command in terms of allowed syntax.
    //! Later, the session iteself will validate the *actual* data passed via cmd.
    auto arg_span = Args::parse_args(argc, argv);
    
    auto current_session = std::make_unique<Session>();

    //! Catch early diagnostics from CLI.
    if(!arg_span.has_value()) {
        current_session->register_diagnostic_rendering(arg_span.error());
        current_session->flush_all_callbacks();
        return;
    }

    //! Validate any input from the command line.
    current_session->validate_arg_span(arg_span.value());

    //! Those diagnostics can manifest during validation
    //! processes, etc...
    if(current_session->caught_early_diagnostics()) {
        current_session->flush_all_callbacks();
        return;
    }

    if(
        arg_span.value().has_flag("--help") ||
        arg_span.value().has_flag("-h")
    ) {
        Interface::help_information();
        return;
    }

    init_build_session(std::move(current_session), [](unique_ptr<Session>& sess) -> State {
        sess->c_state = State::Running;

        auto lexer = Lexer(sess->source.as_str());
        auto lazy_token_stream = lexer.lex_source();

        if(!lexer.diagnostics.empty()) {
            for(const auto& diag : lexer.diagnostics) {
                sess->register_diagnostic_rendering(diag);
            }
            return State::Stopped;
        } else {
            for(const auto& tok : lazy_token_stream.m_tokens) {
                tok->token_to_str();
            }
        }

        return State::Completed;
    });
}

auto Interface::help_information() -> void {
    std::cout << R"(Usage: wombat [OPTIONS] [INPUTS]

Options:
    -b, --build [SOURCE_PATH] <FLAGS>   Compile the source file at SOURCE_PATH.
    -r, --run [SOURCE_PATH] <FLAGS>     Compile and run the executable.
    -u, --outdir [DIR_PATH]             Specify the output directory. 
                                        Defaults to the current directory if omitted.
    -h, --help                          Display this help menu.

Flags:
    -v, --verbose                       Enable detailed process logging.
    -o, --out [EXE_PATH]                Specify the output executable path.
                                        Defaults to the same directory as the source.
)";
}