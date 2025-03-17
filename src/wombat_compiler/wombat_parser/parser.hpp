#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "wtypes.hpp"
#include "diagnostic.hpp"
#include "lex.hpp"
#include "expr.hpp"
#include "ast.hpp"


struct TokenCursor {
    //! Ptr to a token stream.
    //! See [token.hpp]
    Ptr<LazyTokenStream> stream;

    //! 'cur' and 'prev' serve for parsing purposes.
    Ptr<Token> cur;
    Ptr<Token> prev;

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
        if(cur != nullptr) {
            prev = std::move(cur);
        }
        if(auto nxt = stream->eat_one_token(); nxt) {
            cur = mk_ptr<Token>(std::move(nxt.value()));
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
    explicit Parser(
        const SourceCursor& cur,
        const LazyTokenStream& stream
    ) : tok_cur(std::move(stream)), src_cur(std::move(cur)) {
        // Set the parser the the beginning of the stream.
        eat();
    }

    // The whole given token stream into an Ast.
    auto parse() -> AST;

    // Parses a primary expression.
    auto parse_primary() -> Ptr<BaseExpr>;

    // Parses an expression.
    auto parse_float() -> Ptr<BaseExpr>;

    // Parses an expression within a parenthesis.
    auto parse_expr() -> Ptr<BaseExpr>;

    auto convert_expr_to_ast_node(Ptr<BaseExpr>& expr_ref) -> Ptr<AstNode>;

    // Moves forward one token.
    void eat() {
        if(!tok_cur.can_advance()) {
            return;
        }

        // Bump into the next token.
        tok_cur.next();

        // Set position for region parsing.
        src_cur.cur_loc.line = tok_cur.cur->loc.line;
        src_cur.cur_loc.col = tok_cur.cur->loc.col;
    }
    
    void register_warning_diagnostic_pretty(
        std::string message, 
        std::string hint,
        std::vector<Label> labels
    ) {
        diagnostics.emplace_back(
            Diagnostic(Level::Warning, Phase::Parser, message, hint, labels)
        );
    }

    void register_critical_diagnostic_pretty(
        std::string message,
        std::string hint,
        std::vector<Label> labels
    ) {
        diagnostics.emplace_back(
            Diagnostic(Level::Critical, Phase::Parser, message, hint, labels)
        );
    }

    void register_critical_diagnostic_short(std::string message, std::string hint) {
        diagnostics.emplace_back(
            Diagnostic(Level::Critical, Phase::Parser, message, hint, {})
        );
    }
      
    void register_warning_diagnostic_short(std::string message, std::string hint) {
        diagnostics.emplace_back(
            Diagnostic(Level::Warning, Phase::Parser, message, hint, {})
        );
    }

private:
    TokenCursor tok_cur;
    SourceCursor src_cur;
    std::vector<Diagnostic> diagnostics;

    // Looks ahead `n` tokens from the current position, for a certain condition.
    bool ntok_for(int ntok, Closure<bool, Token&> condition) {
        size_t ahead_pos = tok_cur.pos() + ntok;

        if(ahead_pos >= tok_cur.stream_size) {
            WOMBAT_ASSERT(false, "LOOKAHEAD FAILED, OUT OF BOUNDS");
        }
    
        return condition(tok_cur.stream->m_tokens.at(ntok));
    }

    bool is_bin_op(const Token& tok) {
        return tok.matches_any(
            TokenKind::Plus, 
            TokenKind::Minus, 
            TokenKind::Star, 
            TokenKind::Div,
            TokenKind::Precent
        );
    }

    void eat_and_expect(Closure<bool, Token&> condition, std::string expect);

    // Returns a reference to the previous token. 
    inline Token& prev_tok() { 
        return *tok_cur.prev;
    }

    // Returns a reference to the previous token. 
    inline Token& cur_tok() { 
        return *tok_cur.cur;
    }
};

#endif // PARSER_HPP_