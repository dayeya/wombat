#ifndef ARGS_HPP_
#define ARGS_HPP_

#include <iostream>
#include <unordered_set>
#include <string>
#include <optional>
#include <expected>
#include <map>

#include "diagnostic.hpp"

using std::string;
using Enabled = bool;


enum class AvailOpt {
    //! The user specified either '--build' or '-b'
    Build,

    //! The user specified either '--run' or '-r'
    Run,

    //! The user specified either '--help' or '-h'
    Help,

    //! The user specified either '--outfile' or '-o'
    OutFile
};

/**
 * @brief Args class represents the argument parser for the wombat compiler.
 * The up-to-date command for wombat is ` wombat [OPTIONS] [INPUTS] `
 * 
 * *! Non-optional arguments are written inside regular brackets.
 * *! Optionals are written inside angle brackets.
 * 
 * Available options: 
 * 
 *      --build [SOURCE_PATH] <FLAGS>     -> compiles the source_path, customized compilation can be changed with [FLAGS].  
 *      --run, -r [SOURCE_PATH] <FLAGS>   -> compiles and runs the executable, customized compilation can be changed with [FLAGS].
 *      --outdir, -u <DIR_PATH>           -> outputs the output-dir, can be changed if DIR_PATH is given.
 *      --help, -h                        -> displays the help menu 
 * 
 * Available flags:
 *      
 *      --outputfile, -o [EXE_PATH] -> when compiling, create the executable in given path.  
 *      --verbose, -v                   -> detailed process logging.
 */
class Args {
public:
    Args() = default;
    static auto parse_args(int argc, char** argv) -> std::expected<Args, Diagnostic>;
    
    auto is_present(AvailOpt opt) -> bool {
        switch (opt)
        {
            case AvailOpt::Build:
                return has_option_str("--build") || has_option_str("-b");
            case AvailOpt::Run:
                return has_option_str("--run") || has_option_str("-r");
            case AvailOpt::Help:
                return has_option_str("--help") || has_option_str("-h");
            case AvailOpt::OutFile:
                return has_option_str("--outfile") || has_option_str("-o");
            default:
                return false;
        }
    }

    auto get_option_value(AvailOpt opt) const -> std::string {
        switch (opt)
        {
            case AvailOpt::Build:
                return get_option_value_str("--build")
                        .value_or(get_option_value_str("-b").value_or("(NONE)"));
            case AvailOpt::Run:
                return get_option_value_str("--run")
                        .value_or(get_option_value_str("-r").value_or("(NONE)"));
            case AvailOpt::OutFile:
                return get_option_value_str("--outfile")
                        .value_or(get_option_value_str("-o").value_or("(NONE)"));                
            default:
                return "(NONE)";
        }
    }

    auto has_flag(const std::string& flg) const -> bool {
        return flags_.contains(flg);
    }
    
private:
    std::unordered_set<std::string> flags_;
    std::unordered_map<std::string, std::string> options_;

    auto get_option_value_str(const std::string& opt) const -> std::optional<std::string> {
        auto it = options_.find(opt);
        return (it != options_.end()) ? std::optional(it->second) : std::nullopt;
    }

    auto enable_flag(const std::string& flg) -> void {
        flags_.insert(flg);
    }

    auto set_option(const std::string& opt, const std::string& value) -> void {
        options_[opt] = value;
    }

    auto has_option_str(const std::string& opt) const -> bool {
        return options_.contains(opt);
    }

    static bool must_have_value(const std::string& arg) {
        return arg == "--build"  || arg == "-b" ||
               arg == "--run"    || arg == "-r" ||
               arg == "--outdir" || arg == "-u" ||
               arg == "--out"    || arg == "-o";
    }

    static bool is_option(const std::string& opt) {
        return must_have_value(opt);
    }

    static bool is_flag(const std::string& flg) {
        return flg == "--verbose" || flg == "-v" ||
               flg == "--help"    || flg == "-h";
    }
};

#endif // ARGS_HPP_