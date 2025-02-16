#include <iostream>
#include <span>
#include <vector>
#include <optional>
#include <expected>
#include <algorithm>

#include "args.hpp"
#include "emitter.hpp"

auto Args::set_option(std::string opt, std::string value) -> void {
    options_.emplace(
        std::pair<Option, Enabled>({ opt, value }, true)
    );
}

auto Args::has_option(std::string opt) -> bool {
    auto it = std::find_if(options_.begin(), options_.end(), [&opt](const auto& pair) {
        return pair.first.name == opt;
    });
    return it != options_.end();
}

auto Args::get_option_value(std::string opt) -> std::optional<Option> {
    if(!has_option(opt)) return std::nullopt;

    auto it = std::find_if(options_.begin(), options_.end(), [&opt](const auto& pair) {
        return pair.first.name == opt;
    });

    return it->first;
}

auto Args::enable_flag(std::string flg) -> void {
    flags_.insert(std::pair<std::string, Enabled>(flg, true));
}

auto Args::has_flag(std::string flg) -> bool {
    return flags_.find(flg) != flags_.end();
}

auto Args::parse_args(int argc, char** argv) -> std::expected<Args, Diagnostic> {
    // Non-owning stack-allocated viewer.    
    std::span<char*> viewer(argv, argc);

    const auto peek_next_arg = [&viewer](size_t k) {
        return (k + 1 < viewer.size()) ? std::make_optional<std::string>(viewer[k + 1]) : std::nullopt;
    };

    // Zero arguments provided.
    if(argc < 2) {
        return unexpected_diagnostic_from(
            DiagnosticKind::Help,
            { "not enough arguments provided for wombat" },
            { Suggestion("try wombat --help for more information") },
            std::nullopt
        );
    }

    Args wombat_args;
    for(size_t k = 1; k < viewer.size(); ++k) {
        const auto& arg = viewer[k];

        /**
         * [NOTE] this is a military grade arg_parser, which for *now* does *not* handle input validation.
         * Instead, input is validated in each stage. 
         */
        if(Args::is_option(arg)) {
            if(Args::must_have_value(arg)) {
                auto nxt = peek_next_arg(k).value_or("");
                if(nxt != "") {
                    wombat_args.set_option(arg, nxt);
                } else {
                    return unexpected_diagnostic_from(
                        DiagnosticKind::Help,
                        { std::string(arg) + " must be followed by a value" },
                        { Suggestion("try wombat --help for more information") },
                        std::nullopt
                    );
                }
            } else {
                // --outdir was selected.
                // Remember, --outdir is an optional *MUST_BE_FOLLOWED* argument.
                const auto& nxt = peek_next_arg(k).value_or("");
                wombat_args.set_option(arg, nxt);
            }
            k++; // skip
        }
        else if(Args::is_flag(arg)) {
            wombat_args.enable_flag(arg);
        } else {
            return unexpected_diagnostic_from(
                DiagnosticKind::Help,
                { std::string(arg) + " is an unknown argument" },
                { Suggestion("try wombat --help for more information") },
                std::nullopt
            );
        }
    }
    return wombat_args;
}