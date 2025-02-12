#include <iomanip>
#include <string>
#include <expected>
#include <deque>

#include "args.hpp"
#include "emitter.hpp"
#include "callback.hpp"
#include "interface.hpp"

auto WInterface::build_interface(int argc, char** argv) -> WInterface {
    auto wombat_emitter = Emitter();
    std::deque<Callback> wombat_callbacks;

    // Attempt to parse command-line arguments.
    // If parsing fails, recieve the callback and register it. See [callback].
    auto parsed_arguments = Args::parse_args(argc, argv)
        .or_else([&wombat_callbacks, &wombat_emitter](const Diagnostic& diag_from_arg_parsing) -> std::expected<Args, Diagnostic> {
            const auto emit_diag = [&wombat_emitter](const Diagnostic& diag_) { 
                wombat_emitter.emit_diagnostic(diag_); 
            };
            wombat_callbacks.emplace_back(
                CallbackIdentifer::ArgumentDiagnosticCallback, 
                emit_diag, 
                diag_from_arg_parsing
            );
            return Args();
        })
        .value();

    WInterface wombat_interface(
        parsed_arguments,
        wombat_emitter,
        wombat_callbacks
    );

    return wombat_interface;
}

auto WInterface::execute() -> void {
    //! Catch early errors.
    //! They can manifest during input validation or argument parsing.
    while(!callback_stack_.empty()) {
        auto callback = callback_stack_.front();
        callback.invoke();
        callback_stack_.pop_front();
    }
}

auto WInterface::help_information() -> void {
    std::cout << "wombat [OPTIONS] [INPUTS]";
}