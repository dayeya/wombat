#include <iostream>
#include <functional>

#include "args.hpp"
#include "session.hpp"

bool Session::caught_early_diagnostics() {
    if(c_state == State::Running) {
        // early diagnostics occur before loading the "main" function
        // which executes the users command.
        return false;
    } else {
        return session_callbacks.caught_until_now();
    }
}

void Session::validate_arg_span(Args arg_span) {
    if(arg_span.is_present(AvailOpt::Build)) {
        InputFile ifl(arg_span.get_option_value(AvailOpt::Build));
        
        if(auto res = ifl.validate(".wo", true, false); !res) {
            register_diagnostic_rendering(res.error());
        } else {
            source = std::move(ifl);
        }
    } else if(arg_span.is_present(AvailOpt::Run)) {
        InputFile ifl(arg_span.get_option_value(AvailOpt::Run));

        if(auto res = ifl.validate(".wo", true, false); !res) {
            register_diagnostic_rendering(res.error());
        } else {
            source = std::move(ifl);
        }
    } else if(arg_span.is_present(AvailOpt::OutFile)) {
        auto supposed_out_path = arg_span.get_option_value(AvailOpt::OutFile);
        OutputFile ofl(supposed_out_path);

        if(auto res = ofl.validate(".o", false, true); !res) {
            register_diagnostic_rendering(res.error());
        } else {
            executable = std::move(ofl);
        }
    }
}

void Session::register_diagnostic_rendering(const Diagnostic& diag) {
    auto rendering_scheme = (!diag.labels.empty()) ? &Renderer::render_pretty_print : &Renderer::render_short;

    Callback diagnostic_rendering_callback {
        CallbackId::DiagnosticRendering,
        std::bind_front(rendering_scheme, &renderer), diag
    };

    session_callbacks.q.push(diagnostic_rendering_callback); 
}

void Session::flush_all_callbacks() {
    while(!session_callbacks.q.empty()) {
        session_callbacks.q.front().invoke();
        session_callbacks.q.pop();
    }
}

void Session::flush_if_caught_diagnostics() {
    if(session_callbacks.caught_until_now()) {
        flush_all_callbacks();
    }
}

void init_build_session(
    unique_ptr<Session> sess, 
    std::function<State(unique_ptr<Session>&)> behavior
) {
    auto compilation_result = behavior(sess);

    if(compilation_result == State::Stopped) {
        sess->flush_if_caught_diagnostics();
    }
}