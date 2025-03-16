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
#include "callback.hpp"

namespace fs = std::filesystem;

struct SessFile {
    fs::path file;
    fs::perms file_perms;

    SessFile() = default;
    SessFile(std::string path) : file(path), file_perms(fs::perms::none) {};

    inline std::string as_str() { 
        return file.native(); 
    }
    
    auto can_read() -> bool;
    auto can_write() -> bool;

    auto validate(
        const std::string& extension, 
        bool for_read, 
        bool for_write
    ) -> Result<std::monostate, Diagnostic>;
};

//! Aliasing types for cleaner code.
using SessInFile = SessFile;
using SessOutFile = SessFile;

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
    SessInFile source;
    SessOutFile executable;
    SessionCallbacks session_callbacks;

    State c_state;
    Phase c_phase;

    static constexpr int DIAGNOSTIC_CAPACITY = 10;

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
    Ptr<Session> sess, 
    Closure<State, Ptr<Session>&> behavior
);

#endif // SESSION_HPP_
