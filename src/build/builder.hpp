#ifndef BUILDER_HPP_
#define BUILDER_HPP_

#include <span>
#include "alias.hpp"
#include "common.hpp"

constexpr const char* OPTION_PREFIX = "-";

enum class Verbosity: int {
    Quiet = -1,
    Normal,
    Verbose,
    Debug
};

enum class OpCode: int {
    Internal,           // Internal.
    Success,            // Succuessful usage.
    NotEnoughArguments, // Empty shell.
    InvalidArgument,    // Tackeled an unsupported/unknown argument.
    NoInput             // No input was given for a command that requires input.
};

struct BuildConfig {
    StrLoc name;        // Parent executable for builder
    Option<StrLoc> src; // Any input for the compiler to build
    Option<StrLoc> dst; // out location for a single target
    Verbosity verb;     // Verbosity intesity.
    bool compile_only;
    bool compile_and_assemble;
    bool print_ast;
    bool print_tokens;

    BuildConfig() = default;
    BuildConfig(
        StrLoc parent,
        Option<StrLoc> src_location,
        Option<StrLoc> dst_location,
        bool cmpl_only,
        bool cmpl_and_asm,
        bool ast,
        bool tokens,
        Verbosity v
    ) : name{parent},
        src{src_location},
        dst{dst_location},
        compile_only{cmpl_only},
        compile_and_assemble{cmpl_and_asm},
        print_ast{ast},
        print_tokens{tokens},
        verb{v} {}
};

struct Builder {
    BuildConfig config;

    Builder() = default;
    void init(const std::string& parent_exec);
    void exit_builder(OpCode code);

    int stdlib_status_from_op_code(const OpCode& code) const {
        return (code == OpCode::Success) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    void usage(const char* exec) const;
    void version() const;

    void suggest(const std::string& suggestion) const;
    void dump_err_and_exit(OpCode code, const std::string& description, const std::string& suggestion="");

    void parse_arguments(int argc, char** argv);
    void parse_input_file(std::span<char*>& view, size_t& cur, std::string input);
    void parse_out_file(std::span<char*>& view, size_t& cur);
    void parse_option(std::span<char*>& view, size_t& cur, const std::string& opt);

    Option<std::string> next(std::span<char*>& view, size_t& cur);
};

#endif // BUILDER_HPP_