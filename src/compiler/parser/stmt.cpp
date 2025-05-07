#include "parser.hpp"

using ScopeDelimiter = Statement::ScopeDelimiter;
using Keyword = Tokenizer::Keyword;

Statement::If Parser::parse_if_stmt() {
    // Eat the 'if' keyword.
    eat();

    Ptr<BaseExpr> condition = parse_expr_without_recovery();

    ASSERT(
        cur_tok().match_kind(TokenKind::OpenCurly),
        "open a scope after an if statement, use '{}' for this." 
    );
    
    eat();
    Block if_block = parse_block();

    ASSERT(cur_tok().match_kind(TokenKind::CloseCurly), "must close 'if' body with '}'." );
    eat();

    Statement::If if_stmt{};
    if(cur_tok().match_keyword(Keyword::Else)) 
    {
        eat(); // Eat the 'else' keyword.
        ASSERT(
            cur_tok().match_kind(TokenKind::OpenCurly),
            "open a scope for 'else' statement, use '{}' for this." 
        );
        
        eat();
        Block else_block = parse_block();

        ASSERT(cur_tok().match_kind(TokenKind::CloseCurly), "must close 'else' body with '}'." );
        eat();

        if_stmt.has_else = true;
        if_stmt.else_block = std::move(else_block);
    }

    if_stmt.if_block = std::move(if_block);
    if_stmt.condition = std::move(condition);
    return std::move(if_stmt);
}

Statement::Break Parser::parse_break_stmt() {
    // Eat the 'break' keyword.
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::SemiColon),
        "expected ';' after the break keyword." 
    );
    eat();
    return Statement::Break();
}

Statement::Loop Parser::parse_loop_stmt() {
    // Eat the loop keyword.
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::OpenCurly),
        "must open a scope after loop keyword, use '{}' for this." 
    );
    
    eat();
    Block body = parse_block();

    ASSERT(
        cur_tok().match_kind(TokenKind::CloseCurly),
        "must close 'loop' body with '}'." 
    );

    eat();
    Statement::Loop loop{std::move(body)};
    return std::move(loop);
}

Statement::Return Parser::parse_return_stmt() {
    // Eat the 'return' keyword.
    eat();

    if(cur_tok().match_kind(TokenKind::SemiColon)) {
        eat();
        return Statement::Return{current_ctxt, nullptr};
    }

    Statement::Return ret_stmt {
        current_ctxt,
        parse_expr_without_recovery()
    };

    ASSERT(
        ret_stmt.expr != nullptr,
        std::format("expected an expression after 'return' but got '{}'", cur_tok().value)
    );
    ASSERT(
        cur_tok().match_kind(TokenKind::SemiColon),
        std::format("expected ';' after return statement but got '{}'", cur_tok().value)
    );
    eat();

    return std::move(ret_stmt);
}

Statement::Import Parser::parse_import_stmt() {
    // Eat the 'import' keyword.
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier),
        std::format("expected an identifier after 'import' but got '{}'", cur_tok().value)
    );

    Statement::Import import_stmt{
        Identifier(cur_tok().value)
    };

    // Eat the identifier.
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::SemiColon),
        std::format("expected ';' after import statement but got '{}'", cur_tok().value)
    );
    eat();

    return std::move(import_stmt);
}

Statement::Block Parser::parse_block() {
    Statement::Block block{};
    while(
        !cur_tok().match_kind(TokenKind::CloseCurly) &&
        !cur_tok().match_keyword(Keyword::End)
    ) {
        Ptr<Statement::Stmt> stmt = parse_stmt_without_recovery();

        if(cur_tok().match_kind(TokenKind::Eof)) 
        {
            // We reached an end-of-file position, which is invalid.
            ASSERT(false, "must close a scope with either '}' or 'end'");
            return {};
        }        

        block.stmts.push_back(std::move(stmt));
    }
    return std::move(block);
}

Statement::FnCall Parser::parse_fn_call() {
    Statement::FnCall fn_call{expr_ident_fn()};
    ASSERT(
        cur_tok().match_kind(TokenKind::SemiColon),
        std::format("expected ';' after function call but got '{}'", cur_tok().value)
    );
    eat();
    return std::move(fn_call);
}

Ptr<Statement::Stmt> Parser::parse_stmt_without_recovery() {
    if(cur_tok().match_keyword(Keyword::Mut, Keyword::Let)) {
        return mk_ptr(parse_local_decl());
    } 
    if(cur_tok().match_keyword(Keyword::Fn)) {
        return mk_ptr(parse_fn_decl());
    }
    if(cur_tok().match_keyword(Keyword::Return)) {
        return mk_ptr(parse_return_stmt());
    }
    if(cur_tok().match_keyword(Keyword::If)) {
        return mk_ptr(parse_if_stmt());
    }
    if(cur_tok().match_keyword(Keyword::Loop)) {
        return mk_ptr(parse_loop_stmt());
    }
    if(cur_tok().match_keyword(Keyword::Break)) {
        return mk_ptr(parse_break_stmt());
    }
    if(cur_tok().match_keyword(Keyword::Import)) {
        return mk_ptr(parse_import_stmt());
    }
    if(cur_tok().match_kind(TokenKind::Identifier)) {
        // First, check if the 'ident' could be a function call.
        if(
            ntok_for([](Token& tok) {
                return tok.match_kind(TokenKind::OpenParen); 
            }, TokDistance::Next))
        {
            return mk_ptr(parse_fn_call());
        }
        return mk_ptr(parse_local_assignment());
    }
    ASSERT(false, std::format("unknown piece of code, got '{}'", cur_tok().value));
    return nullptr;
}