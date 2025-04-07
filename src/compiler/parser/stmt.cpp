#include "parser.hpp"

using Keyword = Tokenizer::Keyword;

Ptr<Statement::Stmt> Parser::parse_stmt_without_recovery() {
    if(cur_tok().match_keyword(Keyword::Mut, Keyword::Let)) {
        return parse_local_decl();
    } else {
        ASSERT(false, std::format("unknown piece of code: {}", cur_tok().value));
    }
}