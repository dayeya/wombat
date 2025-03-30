#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <iostream>
#include <expected>
#include <queue>
#include <variant>
#include <functional>

#include "args.hpp"
#include "lex.hpp"
#include "token.hpp"
#include "diagnostic.hpp"

namespace fs = std::filesystem;

struct LinkingTarget {
    fs::path from;
    fs::path exe_path;
    bool print_exe_information;

    LinkingTarget() = default;
};

struct AsmTarget {
    fs::path from;
    fs::path asm_path;
    LinkingTarget linking_target;
    bool print_asm_statistics;

    AsmTarget() = default;
};

struct BuildTarget {
    fs::path src_path;
    AsmTarget asm_target;
    bool print_lexical_information;
    bool print_parsing_information;
    bool print_codegen_information;
    bool print_lexical_tokens;
    bool print_parsed_ast;
    bool print_gen_native_instructions;

    BuildTarget() = default;

    inline std::string src_file_name() { 
        return src_path.native(); 
    }
};

enum class State {
    // Indicates that the session has been completed.
    Completed,
    // Indicates that the session has been stopped.
    Stopped,
    // Indicates that the session is currently running.
    Running,
    // Indicates that the session is currently offline.
    Offline
};

class Session {
public:
    BuildTarget build_target;
    Renderer renderer;
    Diagnostics diagnostics;
    State state;

    static constexpr int MAX_OUTPUT_FILE = 100;
    static constexpr int MAX_MODULES = 100;
    static constexpr int MAX_DIAG_CAPACITY = 10;

    Session() 
        : build_target(), 
          renderer(), 
          state{State::Offline}, 
          diagnostics(Session::MAX_DIAG_CAPACITY) {}

    void parse_arguments(int argc, char** argv);
    void init_from_build_targets();

    bool caught_early_diags();
    bool caught_diags();

    void init_build_session(Closure<State, Ptr<Session>&> behavior);

    void add_diagnostic(const Diagnostic& diag);
};

#endif // SESSION_HPP_
