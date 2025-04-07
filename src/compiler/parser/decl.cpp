#include "parser.hpp"

Option<Declaration::Initializer> Parser::parse_local_initializer(Mutability mut) {
    // If we encouter a semi-colon there is not initializer.
    // The parser checks this condition without bumping into the next token.
    if(ntok_for([](Token& tok) { 
        return tok.kind == TokenKind::SemiColon; 
    })) {
        return std::nullopt;
    }

    ASSERT(
        cur_tok().matches_any(
            TokenKind::Eq,
            TokenKind::StarAssign,
            TokenKind::SlashAssign,
            TokenKind::PrecentAssign,
            TokenKind::PlusAssign,
            TokenKind::MinusAssign,
            TokenKind::ShlAssign,
            TokenKind::ShrAssign,
            TokenKind::AmpersandAssign,
            TokenKind::PipeAssign,
            TokenKind::HatAssign
        ),
        std::format(
            "expected an assignment operator but got '{}'", 
            Tokenizer::meaning_from_kind(cur_tok().kind)
        )
    ); 

    Declaration::Initializer init(
        mut,
        Tokenizer::assign_op_from_token(cur_tok()).value()
    );

    // Eat the assignment operator after processing it.
    eat();
    init.expr = std::move(parse_expr_without_recovery());

    ASSERT(
        cur_tok().match_kind(TokenKind::SemiColon), 
        std::format(
            "expected ';' after local declaration but got '{}'",
            Tokenizer::meaning_from_kind(cur_tok().kind)
        )
    );

    return init;
}

Ptr<Declaration::Var> Parser::parse_local_decl() {
    Mutability mut = Declaration::mut_from_token(cur_tok());
    
    // Eat 'let' or 'mut'
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier), 
        std::format(
            "expected an identifier but got '{}'", 
            Tokenizer::meaning_from_kind(cur_tok().kind)
        )
    );

    Expr::Identifier ident(cur_tok().value);

    // Eat the identifier.
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::Colon),
        std::format(
            "expected a colon after identifier but got '{}'", 
            Tokenizer::meaning_from_kind(cur_tok().kind)
        )
    );

    // Eat the colon.
    eat(); 
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier),
        std::format(
            "expected an type-identifier but got '{}'", 
            Tokenizer::meaning_from_kind(cur_tok().kind)
        )
    );

    Expr::Identifier type_ident(cur_tok().value);
    Option<Declaration::Primitive> type = Declaration::try_primitive_from_ident(type_ident); 

    ASSERT(
        type.has_value(),
        std::format("expected a valid primitive type, got '{}'", type_ident.as_str())
    );

    // Eat the type-identifier. (setup for the parsing of the initializer.)
    eat();

    return mk_ptr<Declaration::Var>(
        Declaration::Var(
            StmtKind::Local,
            mut,
            type.value(),
            ident,
            parse_local_initializer(mut)
        )
    );
}