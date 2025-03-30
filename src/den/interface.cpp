#include <iomanip>
#include <string>
#include <expected>
#include <queue>
#include <functional>

#include "interface.hpp"
#include "session.hpp"

void Interface::run(int argc, char** argv) {
    
    Session sess = Session();

    sess.parse_arguments(argc, argv);
    sess.init_sess()

    if(sess.caught_early_diagnostics()) {
        if(sess.sess_opt.verbose) {};   
        return;
    }

    if(
        arg_span.value().has_flag("--help") ||
        arg_span.value().has_flag("-h")
    ) {
        Interface::help_information();
        return;
    }

    init_build_session(std::move(current_session), [](Ptr<Session>& sess) -> State {
        sess->c_state = State::Running;

        auto lexer = Lexer(sess->source.as_str());
        auto lazy_token_stream = lexer.lex_source();

        if(!lexer.diagnostics.empty()) {
            for(const auto& diag : lexer.diagnostics) {
                sess->register_diagnostic_rendering(diag);
            }
            return State::Stopped;
        } else {
            for(const auto& tok : lazy_token_stream.m_tokens) {
                tok.out();
            }
        }

        lazy_token_stream.reset();

        Parser parser(lexer.get_cursor(), lazy_token_stream);
        AST ast = parser.parse();

        PrettyPrintVisitor pp_visitor;

        ast.traverse(pp_visitor);

        return State::Completed;
    });
}