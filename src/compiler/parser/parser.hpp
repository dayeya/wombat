#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "diag.hpp"
#include "lex.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "ast.hpp"
#include "err.hpp"
#include "typing.hpp"

using Statement::Stmt;
using Statement::StmtKind;
using Statement::Block;
using Statement::Import;
using Statement::FnCall;
using Statement::If;
using Statement::Break;
using Statement::Loop;

using Declaration::Parameter;
using Declaration::Var;
using Declaration::Assignment;
using Declaration::FnHeader;
using Declaration::Fn;
using Declaration::Initializer;

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
    // A simple enumeration to represent a distance within the token stream.
    // Those are the most used distances when parsing expressions and statements.
    enum TokDistance: int {
        // The current position.
        Nil = 0,
        // One token from the current position.
        Next = 1
    };

    static CONST int MAX_PARSE_DIAGS = 10;

    Parser(const LazyTokenStream& stream) 
        : current_ctxt{}, tok_cur{std::move(stream)}, diags{MAX_PARSE_DIAGS} {}

    // The whole given token stream into an Ast.
    void parse(AST& ast); 

private:
    Diagnostics diags;
    TokenCursor tok_cur;
    Identifier current_ctxt;

    void eat() {
        if(!tok_cur.can_advance()) {
            return;
        }
        // Bump into the next token.
        tok_cur.next();
    }

    // Looks ahead `n` tokens from the current position, for a certain condition.
    // *defaults* to the next token in line;
    bool ntok_for(Closure<bool, Token&> condition, int ntok) {
        // Increment by 1 since the LazyTokenStream is [-1] indexed.
        size_t ahead_pos = tok_cur.pos() + ntok;
        
        // Reached EOF, finish parsing.
        if(ahead_pos >= tok_cur.stream_size) {
            ASSERT(false, "LOOKAHEAD FAILED: OUT OF BOUNDS");
        }
        
        Token& tok = tok_cur.stream->m_tokens.at(ahead_pos);
        return condition(tok);
    }

    void align_into_begining() {
        ASSERT(tok_cur.cur == nullptr, "parser was already aligned.");
        
        // Align the parser. 
        // Meaning we just set it to the start of the program token stream.
        eat(); 
    }

    inline Token& prev_tok() { 
        return *tok_cur.prev;
    }

    inline Token& cur_tok() { 
        return *tok_cur.cur;
    }

    inline bool unary() {
        return un_op_from_token(cur_tok()) != std::nullopt;
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
    Ptr<Expr::BaseExpr> expr_primary();

    Ptr<Expr::FnCall> expr_ident_fn();
    Ptr<Expr::Local> expr_ident_local();
    Ptr<Expr::UnaryExpr> expr_unary();
    Ptr<Expr::GroupExpr> expr_group();
    Ptr<Expr::Literal> expr_literal();

    // Computes the precedence of the right sibling of the current node.
    // This precedence will be using in the recursive manner when parsing expressions.
    Expr::Precedence rhs_expr_precedence(Tokenizer::BinOpKind binary_op);

    Var parse_local_decl();
    Assignment parse_local_assignment();
    Option<Initializer> parse_local_initializer();

    Ptr<Type> parse_type();
    FnCall parse_fn_call();
    Fn parse_fn_decl();
    Block parse_block();
    Break parse_break_stmt();
    If parse_if_stmt();
    Loop parse_loop_stmt();

    void parse_fn_header_params(FnHeader& header);
    void parse_fn_header(FnHeader& header);

    Statement::Import parse_import_stmt();
    Statement::Return parse_return_stmt();
    Identifier parse_general_ident();
    Option<Parameter> parse_param_within_fn_header();
    Ptr<Stmt> parse_stmt_without_recovery();
    
    // Used for the final AST body.
    Ptr<FnNode> parse_function_to_node();
    Ptr<StmtNode> stmt_to_node(const Ptr<Statement::Stmt>& stmt);
    Ptr<ExprNode> expr_to_node(const Ptr<Expr::BaseExpr>& expr);
};

#endif // PARSER_HPP_