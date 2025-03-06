#include <iostream>
#include "parser.hpp"

bool Parser::ntok_for(int ntok, Closure<bool, Token&> condition) {
    size_t ahead_pos = tok_cur.pos() + ntok;

    if(ahead_pos >= tok_cur.stream_size) {
        static_assert("LOOKAHEAD FAILED, OUT OF BOUNDS");
    }

    return condition(tok_cur.stream->m_tokens.at(ntok));
}

void Parser::eat() {
    if(!tok_cur.can_advance()) {
        return;
    }
    return tok_cur.next();
}

auto Parser::parse_expr() {}