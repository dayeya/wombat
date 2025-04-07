#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "diag.hpp"
#include "lex.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "ast.hpp"
#include "err.hpp"

using Statement::StmtKind;
using Declaration::Mutability;

struct TokenCursor {
    Ptr<Token> cur;
    Ptr<Token> prev;
    size_t stream_size = 0;
    SharedPtr<LazyTokenStream> stream;

    explicit TokenCursor(const LazyTokenStream& stream)
        : stream(std::make_shared<LazyTokenStream>(std::move(stream))), 
          stream_size(stream.self_size()), 
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
        }
    }

    bool can_advance() {
        return stream->has_next();
    }
};

class Parser {
public:

    static CONST int MAX_PARSE_DIAGS = 10;

    explicit Parser(const LazyTokenStream& stream) 
        : tok_cur(std::move(stream)), diags(MAX_PARSE_DIAGS) {}

    // The whole given token stream into an Ast.
    void parse(AST& ast); 

    void eat() {
        if(!tok_cur.can_advance()) {
            return;
        }
        // Bump into the next token.
        tok_cur.next();
    }

private:
    TokenCursor tok_cur;
    Diagnostics diags;

    // Looks ahead `n` tokens from the current position, for a certain condition.
    // *defaults* to the next token in line;
    bool ntok_for(Closure<bool, Token&> condition, int ntok = 1) {
        size_t ahead_pos = tok_cur.pos() + ntok;

        // Reached EOF, finish parsing.
        if(ahead_pos >= tok_cur.stream_size) {
            ASSERT(false, "LOOKAHEAD FAILED: OUT OF BOUNDS");
        }
    
        return condition(tok_cur.stream->m_tokens.at(ntok));
    }

    void align_into_begining() {
        if(tok_cur.cur != nullptr) {
            ASSERT(false, "parser was already aligned.");
        }
        // Align the parser. 
        // Meaning we just set it to the start of the program token stream.
        eat(); 
    }

    bool eat_and_expect(bool condition, std::string expect_err);
    bool eat_and_expect_no_err(bool condition);

    inline Token& prev_tok() { 
        return *tok_cur.prev;
    }

    inline Token& cur_tok() { 
        return *tok_cur.cur;
    }

    inline bool unary() {
        return Tokenizer::un_op_from_token(cur_tok()) != std::nullopt;
    }

    inline bool group_start() {
        return cur_tok().match_kind(TokenKind::OpenParen);
    }

    inline bool group_end() {
        return cur_tok().match_kind(TokenKind::CloseParen);
    }

    inline bool literal() {
        return cur_tok().matches_any(
            TokenKind::LiteralNum, 
            TokenKind::LiteralFloat, 
            TokenKind::LiteralChar, 
            TokenKind::LiteralString, 
            TokenKind::LiteralBoolean
        );
    }

    Ptr<Expr::BaseExpr> parse_expr_without_recovery();
    Ptr<Expr::BaseExpr> expr(Expr::Precedence min_prec);
    Ptr<Expr::UnaryExpr> expr_unary();
    Ptr<Expr::GroupExpr> expr_group();
    Ptr<Expr::Literal> expr_literal();
    Ptr<Expr::BaseExpr> expr_primary();
    Expr::Precedence rhs_expr_precedence(Tokenizer::BinOpKind binary_op);

    Ptr<Statement::Stmt> parse_stmt_without_recovery();
    Ptr<Declaration::Fn> parse_fn_decl();
    Ptr<Declaration::Var> parse_local_decl();
    Option<Declaration::Initializer> parse_local_initializer(Mutability mut);

    Ptr<Node> stmt_to_node(const Ptr<Statement::Stmt>& stmt_ref);
    Ptr<Node> expr_to_node(const Ptr<Expr::BaseExpr>& expr_ref);
};

#endif // PARSER_HPP_