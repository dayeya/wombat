#include "parser.hpp"

using Tokenizer::Token;
using Tokenizer::Keyword;
using Tokenizer::Identifier;

Identifier Parser::parse_general_ident() {
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier),
        std::format("expected identifier but got '{}'", cur_tok().value)
    );

    Identifier ident(cur_tok().value);
    eat();

    return ident;
}

Ptr<Type> Parser::parse_type() {
    if(cur_tok().match_kind(TokenKind::Identifier))
    {
        Identifier ident(cur_tok().value);
        Option<Primitive> type = maybe_primitive(ident.as_str()); 
        ASSERT(
            type.has_value(),
            std::format("expected a valid primitive type, got '{}'", ident.as_str())
        );
        // Eat the type identifier.
        eat();
        Primitive ty = type.value();
        return mk_ptr(PrimitiveType(std::move(ty)));
    }
    if(cur_tok().match_keyword(Keyword::Ptr)) 
    {
        eat();
        ASSERT(
            cur_tok().match_kind(TokenKind::OpenAngle),
            std::format("expected `<` after ptr keyword but got '{}'", cur_tok().value)
        );

        // A pointer underlying type.
        // E.g 'mut string_type_example: ptr<[5]ch>
        eat();
        Ptr<Type> type = parse_type();
        
        ASSERT(
            cur_tok().match_kind(TokenKind::CloseAngle),
            std::format("expected `>` after ptr type but got '{}'", cur_tok().value)
        );
        eat();

        return mk_ptr(PointerType(std::move(type)));
    }
    if(cur_tok().match_kind(TokenKind::OpenBracket)) 
    {
        eat();
        size_t array_size = 0;
        
        ASSERT(
            cur_tok().match_kind(TokenKind::LiteralNum),
            std::format("expected a constant array size but got '{}'", cur_tok().value)
        );

        array_size = std::stoul(cur_tok().value);
        eat();
        
        ASSERT(
            cur_tok().match_kind(TokenKind::CloseBracket),
            std::format("expected `]` after array type but got '{}'", cur_tok().value)
        );
        eat();

        Ptr<Type> type = parse_type();
        return mk_ptr(ArrayType(std::move(array_size), std::move(type))); 
    }
    ASSERT(
        false,
        std::format("expected type but got '{}'", cur_tok().value)
    );
    return nullptr;
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
            Tokenizer::tok_kind_str(cur_tok().kind)
        )
    );

    // Build and eat the identifier.
    Identifier param_ident(cur_tok().value);
    eat();

    ASSERT(
        cur_tok().match_kind(TokenKind::Colon),
        std::format("expected ':' after param identifier but got '{}'", cur_tok().value)
    );
    eat();

    return Parameter(
        std::move(mut),
        std::move(param_ident),
        parse_type()
    );
}

void Parser::parse_fn_header_params(FnHeader& header) {
    if(cur_tok().match_kind(TokenKind::CloseParen)) {
        // We empty parameter list.
        header.params = std::vector<Parameter>{};
    }
    else 
    {
        while(true)
        {
            Option<Parameter> param = parse_param_within_fn_header();

            // Why should the parser be crashed?
            // If the `parse_param_within_fn_header` fails somehow, we cover it.
            ASSERT(param.has_value(), "unexpected behavior");
            header.params.push_back(std::move(param.value()));
            
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
    Ptr<Type> type = parse_type();
    Identifier ident = parse_general_ident();
    
    ASSERT(
        cur_tok().match_kind(TokenKind::OpenParen), 
        std::format(
            "expected '(' after in fn declaration but got '{}'",
            Tokenizer::tok_kind_str(cur_tok().kind)
        )
    );    
    
    current_ctxt.set(ident.as_str());
    header.ident = std::move(ident);
    header.ret_type = std::move(type);

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
        std::format("expected 'end' keyword but got '{}'", Tokenizer::tok_kind_str(cur_tok().kind))
    );
    eat();

    return Fn(header, body);
}

Option<Initializer> Parser::parse_local_initializer() {
    // If we encouter a semi-colon there is not initializer.
    // The parser checks this condition without bumping into the next token.
    if(cur_tok().match_kind(TokenKind::SemiColon)) {
        eat();
        return std::nullopt;
    }
    
    ASSERT(
        cur_tok().match_kind(TokenKind::Eq),
        std::format("expected a '=' but got '{}'", cur_tok().value)
    );
    auto assign_op = assign_op_from_token(cur_tok()).value();

    // Eat the assignment operator after processing it.
    eat();
    Initializer init(assign_op, parse_expr_without_recovery());

    ASSERT(
        cur_tok().match_kind(TokenKind::SemiColon), 
        std::format(
            "expected ';' after local declaration but got '{}'",
            Tokenizer::tok_kind_str(cur_tok().kind)
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
            Tokenizer::tok_kind_str(cur_tok().kind)
        )
    );

    Mutability mut = Declaration::mut_from_token(mut_token);
    Identifier ident(cur_tok().value);

    // Eat the identifier.
    eat();
    ASSERT(
        cur_tok().match_kind(TokenKind::Colon),
        std::format(
            "expected a colon after identifier but got '{}'", 
            Tokenizer::tok_kind_str(cur_tok().kind)
        )
    );

    // Eat the colon.
    eat(); 
    Ptr<Type> type = parse_type();
    Option<Initializer> init = parse_local_initializer();

    return Var(
        std::move(mut),
        std::move(ident),
        std::move(type),
        std::move(init)
    );
}

Assignment Parser::parse_local_assignment() {
    ASSERT(
        cur_tok().match_kind(TokenKind::Identifier),
        std::format(
            "expected an identifier but got '{}'", 
            Tokenizer::tok_kind_str(cur_tok().kind)
        )
    );
    Identifier lvalue(cur_tok().value);

    eat();
    Option<Initializer> init = parse_local_initializer();
    ASSERT(
        init.has_value(),
        std::format(
            "expected an assignment operator or but got '{}'", 
            tok_kind_str(cur_tok().kind)
        )
    );

    return Assignment(std::move(lvalue), std::move(init.value()));
}