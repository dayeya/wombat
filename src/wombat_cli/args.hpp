#ifndef ARGS_HPP_
#define ARGS_HPP_

#include <iostream>
#include <string>
#include <optional>
#include <expected>
#include <map>

#include "diagnostic.hpp"

using std::string;

using Enabled = bool;

/**
 * @brief Option class for storing data passed to the interface.
 * e.g [ wombat --build <PATH> ]
 *               <name> <param>
 */
struct Option {
    string name;
    string value;

    bool operator<(const Option& other) const {
        return name < other.name;
    }
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
    auto show() -> void;
    static auto parse_args(int argc, char** argv) -> std::expected<Args, Diagnostic>;

private:
    std::map<string, Enabled> flags_;
    std::map<Option, Enabled> options_;

    auto set_option(string opt, std::string value) -> void;
    auto has_option(string opt) -> bool;
    auto enable_flag(string flg) -> void;
    auto has_flag(string flg) -> bool;
    auto has_required_value(string opt) -> std::optional<Option>;

    /**
     * @brief Checks if the given [arg] is an argument that *must* be followed by a value.
     */
    static bool must_have_value(string arg) {
        return (
            arg == "--build"  || arg == "-b" ||
            arg == "--run"    || arg == "-r"
        );
    }

    static bool is_option(std::string opt) {
        return (
            opt == "--build"  || opt == "-b" ||
            opt == "--run"    || opt == "-r" ||
            opt == "--outdir" || opt == "-u"
        );
    }

    static bool is_flag(string flg) {
        return (
            flg == "--verbose" || flg == "-v" ||
            flg == "--help"    || flg == "-h" 
        );
    }
};

#endif // ARGS_HPP_
