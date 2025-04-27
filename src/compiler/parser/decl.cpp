#include "parser.hpp"

using Declaration::try_primitive_from_ident;

Expr::Identifier Parser::parse_general_ident() {
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier),
        std::format("expected function name identifier but got '{}'", cur_tok().value)
    );

    Expr::Identifier fn_ident(cur_tok().value);
    eat();

    return fn_ident;
}

Option<Primitive> Parser::parse_type_ident() {
    Expr::Identifier ident(cur_tok().value);
    auto type = try_primitive_from_ident(ident); 
    ASSERT(
        type.has_value(),
        std::format("expected a valid primitive type, got '{}'", ident.as_str())
    );
    // Eat the type identifier.
    eat();
    return type;
}

Option<Parameter> Parser::parse_param_within_fn_header() {
    Mutability mut = Mutability::Immutable;
    if(cur_tok().match_keyword(Tokenizer::Keyword::Mut)) 
    {   
        eat();
        mut = Mutability::Mutable;
    }

    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier), 
        std::format(
            "expected an identifier but got '{}'", 
            Tokenizer::meaning_from_kind(cur_tok().kind)
        )
    );

    // Build and eat the identifier.
    Expr::Identifier param_ident(cur_tok().value);
    eat();

    ASSERT(
        cur_tok().match_kind(TokenKind::Colon),
        std::format("expected ':' after param identifier but got '{}'", cur_tok().value)
    );
    eat();

    return Parameter(
        mut,
        parse_type_ident().value(), 
        param_ident
    );
}

void Parser::parse_fn_header_params(FnHeader& header) {
    if(cur_tok().match_kind(TokenKind::CloseParen)) {
        // We empty parameter list.
        header.params = {};
    }
    else 
    {
        while(true)
        {
            Option<Parameter> param = parse_param_within_fn_header();

            // Why should the parser be crashed?
            // If the `parse_param_within_fn_header` fails somehow, we cover it.
            ASSERT(param.has_value(), "unexpected behavior");

            // Just add the parameter. (We will build the signature at the next stage.)
            header.params.push_back(param.value());
            header.sig.argument_types.push_back(param.value().type);
            
            if(cur_tok().match_kind(TokenKind::CloseParen)) {
                break;
            }
            
            ASSERT(
                cur_tok().match_kind(TokenKind::Comma),
                std::format("expected a comma or a closing parenthesis after function parameter but got '{}'", cur_tok().value)
            );        
            eat();
        }
    }
}

void Parser::parse_fn_header(FnHeader& header) {
    Option<Primitive> type = parse_type_ident();
    Expr::Identifier ident = parse_general_ident();
    
    ASSERT(
        cur_tok().match_kind(TokenKind::OpenParen), 
        std::format(
            "expected '(' after in fn declaration but got '{}'",
            meaning_from_kind(cur_tok().kind)
        )
    );    

    header.ident = ident;
    header.sig.ret_type = type.value();

    eat();
    parse_fn_header_params(header);
}

Fn Parser::parse_fn_decl() {
    // Eat the 'fn' keyword and bump into the function return type.
    eat();

    FnHeader header;
    parse_fn_header(header);
    
    // Eat the closing parenthesis.
    eat();
    Statement::Block body = parse_block();

    ASSERT(
        !body.stmts.empty(), 
        "cannot define function without a body"
    );

    ASSERT(
        cur_tok().match_keyword(Tokenizer::Keyword::End), 
        "expected 'end' keyword but got '{}'", meaning_from_kind(cur_tok().kind)
    );
    eat();

    return Fn(header, body);
}

Option<Initializer> Parser::parse_local_initializer() {
    // If we encouter a semi-colon there is not initializer.
    // The parser checks this condition without bumping into the next token.
    if(cur_tok().match_kind(TokenKind::SemiColon)) {
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
            meaning_from_kind(cur_tok().kind)
        )
    );
    auto assign_op = assign_op_from_token(cur_tok()).value();

    // Eat the assignment operator after processing it.
    eat();
    Initializer init(assign_op, parse_expr_without_recovery());

    ASSERT(
        cur_tok().match_kind(TokenKind::SemiColon), 
        std::format(
            "expected ';' after local declaration but got '{}'",
            meaning_from_kind(cur_tok().kind)
        )
    );

    eat();
    return init;
}

Var Parser::parse_local_decl() {
    // Save the mutability modifier.
    Token& mut_token = cur_tok();
    
    // Eat 'let' or 'mut'
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier), 
        std::format(
            "expected an identifier but got '{}'", 
            meaning_from_kind(cur_tok().kind)
        )
    );

    Mutability mut = Declaration::mut_from_token(mut_token);
    Expr::Identifier ident(cur_tok().value);

    // Eat the identifier.
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::Colon),
        std::format(
            "expected a colon after identifier but got '{}'", 
            meaning_from_kind(cur_tok().kind)
        )
    );

    // Eat the colon.
    eat(); 
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier),
        std::format(
            "expected an type-identifier but got '{}'", 
            meaning_from_kind(cur_tok().kind)
        )
    );
    
    auto type = parse_type_ident();

    return Var(
        mut,
        type.value(),
        ident,
        parse_local_initializer()
    );
}

Assignment Parser::parse_local_assignment() {
    Expr::Identifier ident(cur_tok().value);
    eat();

    Option<Initializer> init = parse_local_initializer();
    ASSERT(
        init.has_value(),
        std::format(
            "expected an assignment operator or but got '{}'", 
            meaning_from_kind(cur_tok().kind)
        )
    );

    return Assignment(std::move(ident), std::move(init.value()));
}