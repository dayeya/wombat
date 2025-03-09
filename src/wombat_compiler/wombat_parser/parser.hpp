#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "wtypes.hpp"
#include "expr.hpp"
#include "ast.hpp"


struct TokenCursor {
    //! Ptr to a token stream.
    //! See [token.hpp]
    SmartPtr<LazyTokenStream> stream;

    //! 'cur' and 'prev' serve for parsing purposes.
    SmartPtr<Token> cur;
    SmartPtr<Token> prev;

    //! Total size of tokens inside 'stream'.
    size_t stream_size = 0;

    explicit TokenCursor(LazyTokenStream s)
        : stream(std::make_unique<LazyTokenStream>(s)), 
          stream_size(stream->m_tokens.size()), 
          cur(nullptr), 
          prev(nullptr) {}

    inline int pos() {
        return stream->cur;
    }

    void next() {
        if(cur) {
            prev = std::move(cur);
        }
        if(auto nxt = stream->eat_one_token(); nxt) {
            cur = std::make_unique<Token>(nxt.value());
        } else {
            cur = nullptr;
            stream->state = TState::Ended;
        }
    }

    bool can_advance() {
        return stream->has_next();
    }
};

class Parser {
public:
    TokenCursor tok_cur;

    explicit Parser(
        const LazyTokenStream& stream
    ) : tok_cur(std::move(stream)) {};

    //! The whole given token stream into an Ast.
    auto parse() -> AST;

    //! Parses an expression.
    auto parse_expr();

    //! Moves forward one token.
    void eat();

private:
    //! Looks ahead `n` tokens from the current position, for a certain condition.
    bool ntok_for(int ntok, Closure<bool, Token&> condition);
};

#endif // PARSER_HPP_