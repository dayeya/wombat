#ifndef SESSION_HPP_
#define SESSION_HPP_

#include <iostream>
#include <expected>
#include <queue>
#include <functional>

#include "argp.h"
#include "io_file.hpp"
#include "lex.hpp"
#include "token.hpp"
#include "diagnostic.hpp"
#include "callback.hpp"

struct SessionCallbacks {
    std::queue<Callback> q;

    SessionCallbacks() : q() {};

    bool caught_until_now() {
        return !q.empty();
    }
};

enum class State {
    //! Indicates that the session has been completed.
    Completed,

    //! Indicates that the session has been stopped.
    //! A session might be stopped if any erros have been detected and returned as diagnostics.
    //! In that case the session will abort, release any diagnostics to the user and quit the compiler.
    Stopped,

    //! Indicates that the session is currently running.
    Running,

    //! Indicates that the session is currently offline.
    Offline
};

class Session {
public:
    Renderer renderer;
    InputFile source;
    OutputFile executable;
    SessionCallbacks session_callbacks;

    State c_state;
    Phase c_phase;

    Session()
        : renderer(), 
          source(), 
          executable(), 
          session_callbacks(), 
          c_state(State::Offline),
          c_phase(Phase::SessInit) {};

    void flush_all_callbacks();
    void flush_if_caught_diagnostics();
    void validate_arg_span(Args arg_span);
    bool caught_early_diagnostics();

    void register_diagnostic_rendering(const Diagnostic& diag);
};

void init_build_session(
    unique_ptr<Session> sess, 
    std::function<State(unique_ptr<Session>&)> behavior
);

#endif // SESSION_HPP_
