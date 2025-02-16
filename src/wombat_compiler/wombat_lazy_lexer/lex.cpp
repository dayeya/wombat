#include <iostream>
#include <optional>
#include <expected>

#include "lex.hpp"
#include "diagnostic.hpp"
#include "token.hpp"

using std::unique_ptr, std::make_unique;
using std::shared_ptr, std::make_shared;

bool Lexer::lexer_is_new_line(char c) {
  return c == '\n';
}

bool Lexer::lexer_is_alnum(char c) {
  return std::isalnum(static_cast<unsigned char>(c));
}

bool Lexer::lexer_is_digit(char c) {
  return std::isdigit(static_cast<unsigned char>(c));
}

bool Lexer::lexer_is_alpha(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::lexer_is_symbol(char c) {
  return std::ispunct(static_cast<unsigned char>(c));
}

auto Lexer::advance_cursor() -> char {
  source.get(m_cur);
  if(lexer_is_new_line(m_cur)) {
      m_line++;
      m_col = 0; // set col_ BEFORE the start of the line 
  } else {
      m_col++;
  }
  return m_cur;
};

auto Lexer::peek_next() -> int {
  return source.peek();
};


void Lexer::skip_whitespaces() {
  auto next_ch = peek_next();
  while(!source.eof() && std::isspace(static_cast<unsigned char>(next_ch))) {
      advance_cursor();
      next_ch = peek_next();
  }
}

bool Lexer::lexer_is_readable(std::string iden) {
}

void Lexer::assign_token(Token* token, std::string text, TokenKind kind) {
  token->value = text;
  token->kind = kind;
}

void Lexer::lex_eof(Token* token) {
  token->kind = TokenKind::Eof;
}

void Lexer::lex_foreign(Token* token, char ch) {
  token->extend(ch);
  token->kind = TokenKind::Foreign;
}

void Lexer::lex_line_comment(Token* token) {
  token->extend(m_cur);
  token->extend(advance_cursor());

  advance_cursor();
  while(!source.eof() && !lexer_is_new_line(peek_next())) {
    token->extend(advance_cursor());
  }

  token->kind = TokenKind::LineComment;
}

auto Lexer::lex_word(Token* token) -> std::expected<Token*, Diagnostic> {
  token->extend(m_cur);  

  while(!source.eof() && lexer_is_alpha(peek_next())) {
    token->extend(advance_cursor());
  }

  //! could be a readble
  if(peek_next() == '!') {
    token->extend(advance_cursor());
    if(lexer_is_readable(token->value)) token->kind = TokenKind::Readable;
  } else {
    // If keyword
  }

  token->kind = TokenKind::Identifier;
}

auto Lexer::lex_literal(Token* token) -> std::expected<Token*, Diagnostic> {
  token->extend(m_cur);

  // Lexes a numerical literal, either an integer or a float.
  auto lex_numerical_literal = [&]() -> std::expected<Token*, Diagnostic> {
    while(!source.eof() && lexer_is_digit(peek_next())) {
      token->extend(advance_cursor());
    }
    
    // Lexing a float.
    if(peek_next() == '.') {
      token->extend(advance_cursor());
      if (!lexer_is_digit(peek_next())) {
          return unexpected_diagnostic_from(
            DiagnosticKind::Critical,
            { "character after radix point is not a digit, found " + std::string{static_cast<char>(peek_next())} },
            {}, //! no suggestions for now.
            CodeLocation(file_name, m_line, m_col)
          );
      } else {
          while (!source.eof() && lexer_is_digit(peek_next())) {
              token->extend(advance_cursor());
          }
          token->kind = TokenKind::Literal; // Float literal.
      } 
    } else {
      token->kind = TokenKind::Literal; // Integer literal.
    }
    return token;
  };

  //! Lexes a string literal.
  //! [TODO]: Handle situations like:
  //! 
  //! let message: String = "hello, my"name is"daniel";
  //! 
  //! We should allow escaping (using '\'). But if escaping is NOT used we should return a diagnostic. 
  auto lex_string_literal = [&]() -> std::expected<Token*, Diagnostic> {
    while(!source.eof() && peek_next() != '"') {
      token->extend(advance_cursor());
    }
    token->kind = TokenKind::Literal; // String-Literal.
    return token;
  };

  // Lexes a char literal. e.g 'a'.
  auto lex_char_literal = [&]() -> std::expected<Token*, Diagnostic> {
    while(!source.eof() && peek_next() != '\'') {
      token->extend(advance_cursor());
    }
    token->kind = TokenKind::Literal;
    return token;
  };

  // Lexes a boolean literal. Can be 'true' or 'false'.
  auto lex_boolean_literal = [&]() -> std::expected<Token*, Diagnostic> {
    while(
      !source.eof() && 
      lexer_is_alpha(peek_next()) &&
      (token->value != "true" || token->value != "false")
    ) {
      token->extend(advance_cursor());
    }
    token->kind = TokenKind::Literal; // Boolean literal.
    return token;
  };

  if(lexer_is_digit(m_cur)) return lex_numerical_literal();
  else if(m_cur == '\'') return lex_char_literal();
  else if(m_cur == '"') return lex_string_literal();
  else return lex_boolean_literal();
}

auto Lexer::lex_symbol(Token* token) -> std::expected<Token*, Diagnostic> {
  if(m_cur == '(') {
    assign_token(token, "(", TokenKind::OpenParen);
  } else if(m_cur == ')') {
    assign_token(token, ")", TokenKind::CloseParen);
  } else if(m_cur == '{') {
    assign_token(token, "{", TokenKind::OpenCurly);
  } else if(m_cur == '}') {
    assign_token(token, "}", TokenKind::CloseCurly);
  } else if(m_cur == '[') {
    assign_token(token, "[", TokenKind::OpenBracket);
  } else if(m_cur == ']') {
    assign_token(token, "]", TokenKind::CloseBracket);
  } else if(m_cur == ':') {
    assign_token(token, ":", TokenKind::Colon);
  } else if(m_cur == ';') {
    assign_token(token, ";", TokenKind::SemiColon);
  } else if(m_cur == '<') {
    if(peek_next() == '=') {
      assign_token(token, "<=", TokenKind::Le);
      advance_cursor();
    } else {
      assign_token(token, "<", TokenKind::OpenAngle);
    }
  } else if(m_cur == '>') {
    if(peek_next() == '=') {
      assign_token(token, ">=", TokenKind::Ge);
      advance_cursor();
    } else {
      assign_token(token, ">", TokenKind::CloseAngle);
    }
  } else if(m_cur == '-') {
    if(peek_next() == '>') {
      assign_token(token, "->", TokenKind::ReturnSymbol);
      advance_cursor();
    } else {
      assign_token(token, "-", TokenKind::Minus);
    }
  } else if(m_cur == '+') {
    assign_token(token, "+", TokenKind::Plus);
  } else if(m_cur == '=') {
    if(peek_next() == '=') {
      assign_token(token, "==", TokenKind::DoubleEq);
      advance_cursor();
    } else {
      assign_token(token, "=", TokenKind::Eq);
    }
  } else {
    // Invalid symbol
    assign_token(token, std::string{m_cur}, TokenKind::Foreign);
  }
}

auto Lexer::lexer_next_token(TokenStream* token_stream, Token* token) -> std::expected<Token*, Diagnostic> {
  token->clean();
  skip_whitespaces();
  advance_cursor();

  // Attach text to the token.
  if(source.eof()) lex_eof(token);
  else if(m_cur == '/' && peek_next() == '/') lex_line_comment(token);
  else if(lexer_is_alpha(m_cur)) lex_word(token);
  else if(lexer_is_digit(m_cur) || m_cur == '"') lex_literal(token);
  else if(lexer_is_symbol(m_cur)) lex_symbol(token);
  else { 
    lex_foreign(token, m_cur); 
  };

  // Attach unique meaning to the token

  token_stream->push_token(*token);
}

auto Lexer::lex_source() -> TokenStream {
  Token cur_token;
  TokenStream token_stream{};

  lexer_next_token(&token_stream, &cur_token);
  while(!cur_token.compare_kind(TokenKind::Eof)) {
    lexer_next_token(&token_stream, &cur_token);
  }

  return token_stream;
};

void Lexer::output_token(const Token& token) {
  std::cout << "WOMBAT::Token { text: " << token.value 
            << ", kind: " + kind_to_str(token.kind)
            << " };" << "\n"; 
}

auto Lexer::kind_to_str(const TokenKind& kind) -> std::string {
  switch (kind) {
      case TokenKind::OpenParen:       
        return "Open_Paren";
      case TokenKind::CloseParen:      
        return "Close_Paren";
      case TokenKind::OpenBracket:     
        return "Open_Bracket";
      case TokenKind::CloseBracket:    
        return "Close_Bracket";
      case TokenKind::OpenCurly:       
        return "Open_Curly";
      case TokenKind::CloseCurly:
        return "Close_Curly";
      case TokenKind::OpenAngle:       
        return "Open_Angle";
      case TokenKind::CloseAngle:      
        return "Close_Angle";
      case TokenKind::Lt:              
        return "Less_Then";
      case TokenKind::Gt:              
        return "Greater_Then";
      case TokenKind::ReturnSymbol:           
        return "Arrow";
      case TokenKind::Minus:
        return "Minus_Operator";
      case TokenKind::Plus:            
        return "Plus_Operator";
      case TokenKind::DoubleEq:  
        return "Equals_Assigment";
      case TokenKind::Eq: 
        return "Equals_Statement";
      case TokenKind::Le:              
        return "Less_Then_Or_Equal_To";
      case TokenKind::Ge:              
        return "Greater_Then_Or_Equal_To";
      case TokenKind::Eof:             
        return "End_Of_File";
      case TokenKind::Colon:
        return "Colon";
      case TokenKind::SemiColon:
        return "Semi_Colon";   
      case TokenKind::Literal:    
        return "Literal";
      case TokenKind::Identifier:
        return "Identifier";
      case TokenKind::Whitespace:      
        return "Whitespace";
      case TokenKind::LineComment:
        return "Single_Line_Comment";
  }
  return "Foreign_Token";
}