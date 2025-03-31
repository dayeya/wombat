#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <iostream>
#include <expected>
#include <queue>
#include <variant>
#include <functional>

#include "alias.hpp"
#include "builder.hpp"
#include "diagnostic.hpp"

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

class Compiler {
public:
    Renderer renderer;
    Diagnostics diagnostics;
    State state;

    static constexpr int MAX_OUTPUT_FILE = 100;
    static constexpr int MAX_MODULES = 100;
    static constexpr int MAX_DIAG_CAPACITY = 10;

    Compiler()
        : renderer(), 
          state{State::Offline}, 
          diagnostics(Compiler::MAX_DIAG_CAPACITY) {}

    void init_config(const BuildConfig& config);
    void compile_target();
    
private:
    bool caught_diags();
    bool caught_early_diags();

    void finish();
    void build_target_into_exectuable();
    void add_diagnostic(const Diagnostic& diag);
};

#endif // SESSION_HPP_
