#include <iostream>
#include <span>
#include <vector>
#include <optional>
#include <expected>
#include <unordered_map>
#include <unordered_set>

#include "args.hpp"

auto Args::parse_args(int argc, char** argv) -> std::expected<Args, Diagnostic> {
    std::span<char*> viewer(argv, argc);

    const auto peek_next_arg = [&viewer](size_t k) {
        return (k + 1 < viewer.size()) ? std::make_optional<std::string>(viewer[k + 1]) : std::nullopt;
    };

    if (argc < 2) {
        Diagnostic diag {
            Level::Critical,
            Phase::Precomp,
            "not enough arguments provided",
            "use `wombat --help` for more information",
            {}
        };
        return std::unexpected<Diagnostic>{diag};
    }

    Args wombat_args;
    for (size_t k = 1; k < viewer.size(); ++k) {
        const auto& arg = viewer[k];

        if (Args::is_option(arg)) {
            auto nxt = peek_next_arg(k);
            if (nxt) {
                wombat_args.set_option(arg, *nxt);
                k++; // Skip value
            } else {
                Diagnostic diag {
                    Level::Critical,
                    Phase::Precomp,
                    std::string(arg) + " must be followed by a value",
                    "use `wombat --help` for more information",
                    {}
                };
                return std::unexpected<Diagnostic>{diag};
            }
        } else if (Args::is_flag(arg)) {
            wombat_args.enable_flag(arg);
        } else {
            Diagnostic diag {
                Level::Critical,
                Phase::Precomp,
                std::string(arg) + " is an unknown argument",
                "use `wombat --help` for more information",
                {}
            };
            return std::unexpected<Diagnostic>{diag};
        }
    }
    return wombat_args;
}
