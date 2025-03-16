#include <iostream>
#include <functional>
#include <expected>

#include "args.hpp"
#include "session.hpp"

auto SessFile::can_read() -> bool {
    return (file_perms & fs::perms::owner_read) != fs::perms::none;
}

auto SessFile::can_write() -> bool {
    return (file_perms & fs::perms::owner_write) != fs::perms::none;
}

auto SessFile::validate(
    const std::string& extension, 
    bool for_read, 
    bool for_write
) -> std::expected<std::monostate, Diagnostic> {
    if(!fs::exists(file)) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            file.generic_string() + " does not exist", 
            "provide an existing file location", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    if(fs::is_directory(file)) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            file.generic_string() + " is a directory", 
            "provide an existing, valid .wo source file", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    if(file.extension() != extension) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            file.extension().generic_string() + " is not the expected extension, use " + extension, 
            "see wombat-docs for more information", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    //! file was empty.
    //! Can occur if component-validation on file failed.  
    if(file.empty()) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            "cannot operate on an empty path", 
            "see wombat-docs for more information", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    // Given file is a valid wombat-source-file
    file_perms = fs::status(file).permissions();

    //! not-allowed to read from file.
    if(for_read && !can_read()) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            "file permissions denied, canonot read from " + file.filename().generic_string(), 
            "change permissions to allow reading", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    //! not-allowed to write to file.
    if(for_read && !can_write()) {
        auto diag = Diagnostic(
            Level::Critical, Phase::Precomp,
            "file permissions denied, canonot write from " + file.filename().generic_string(), 
            "change permissions to allow writing", {}
        );
        return std::unexpected<Diagnostic>{diag};
    }

    // All validation were successful.
    return std::monostate{};
}

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
        SessInFile ifl(arg_span.get_option_value(AvailOpt::Build));
        
        if(auto res = ifl.validate(".wo", true, false); !res) {
            register_diagnostic_rendering(res.error());
        } else {
            source = std::move(ifl);
        }
    } else if(arg_span.is_present(AvailOpt::Run)) {
        SessInFile ifl(arg_span.get_option_value(AvailOpt::Run));

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
    auto rendering_scheme = !diag.labels.empty() ? &Renderer::render_pretty_print : &Renderer::render_short;

    auto diagnostic_rendering_callback = Callback {
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
    Ptr<Session> sess, 
    Closure<State, Ptr<Session>&> behavior
) {
    auto compilation_result = behavior(sess);

    if(compilation_result == State::Stopped) {
        sess->flush_if_caught_diagnostics();
    }
}