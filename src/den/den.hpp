#ifndef DEN_HPP_
#define DEN_HPP_

#include <span>
#include "alias.hpp"
#include "common.hpp"

using ArgViewer = std::span<char*>;

constexpr const char* OPTION_PREFIX = "-";

enum class VerbosityLevel: int {
    Verbose,
    Normal,
    HighVerbose,
    Debug,
    Quiet
};

enum class DenCode: int {
    DenSuccess,         // Succuessful usage.
    NotEnoughArguments, // Empty shell.
    InvalidArgument,    // Tackeled an unsupported/unknown argument.
    NoInput             // No input was given for a command that requires input.
};

enum class DenCommand: int {
    Missing,    // Missing
    Build,
    Run,
    Help
};

enum class DenOption: int {
    OutputLoc,
    Silent,
    Verbosity
};

enum class ParseTime: int {
    Optional, 
    Command,
    None
};

struct DenOptions {
    std::string name;   // Parent .exe file. 
    DenCommand cmd;     // The command to be executed.
    std::string input;  // Any input for the given command.
    VerbosityLevel verb;
    std::string out_location;

    DenOptions() = default;

    void init(std::string parent_exec) {
        name = parent_exec;
        cmd = DenCommand::Missing;
        input = "";
        verb = VerbosityLevel::Normal;
        out_location = "";
    }

    std::string command_meaning() const {
        switch (cmd)
        {
            case DenCommand::Run: 
                return "‘run’";
            case DenCommand::Build:
                return "‘build’";
            case DenCommand::Help: 
                return "help’";
            default: {
                UNREACHABLE();    
            }
        }
    }
};

struct Den {
    DenOptions den_options;
    ParseTime parse_time;

    Den() = default;
    void init(std::string parent_exec);
    void clean();

    void den_exit(const DenCode& code);
    void den_show_error_diagnostic(const DenCode& code);
    void den_suggest(const std::string& suggestion);
    void den_usage(const char* exec, VerbosityLevel verb) const;

    void parse(int argc, char** argv);
    void parse_cmd(ArgViewer& view, size_t& cur, std::string arg);
    void parse_cmd_input(ArgViewer& view, size_t& cur);

    void parse_option(ArgViewer& view, size_t& cur, std::string opt);
    void parse_option_value(ArgViewer& view, size_t& cur, std::string opt);
    Option<std::string> next(ArgViewer& view, size_t& k);
};

#endif // DEN_HPP_