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
}

auto Lexer::peek_next() -> int {
  return source.peek();
}

void Lexer::skip_whitespaces() {
  auto next_ch = peek_next();
  while(
    !source.eof() && 
    std::isspace(static_cast<unsigned char>(next_ch))
  ) {
      advance_cursor();
      next_ch = peek_next();
  }
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

  while(
    !source.eof() && 
    (lexer_is_alnum(peek_next()) || peek_next() == '_')
  ) {
    token->extend(advance_cursor());
  }

  //! Assuming that the current token is a readable, if it is followed by a '!';
  if(peek_next() == '!') {
    token->extend(advance_cursor());
    token->kind = TokenKind::Readable;
    return token;
  } else {
    if(
      token->value == "func"   ||
      token->value == "endf"   ||
      token->value == "let"    ||
      token->value == "if"     ||
      token->value == "else"   ||
      token->value == "return" ||

      token->value == "int"    ||
      token->value == "float"  ||
      token->value == "ch"     ||
      token->value == "String" ||
      token->value == "bool"   ||
      token->value == "ref"
    ) {
      token->kind = TokenKind::Keyword;
      return token;
    }
  }

  token->kind = TokenKind::Identifier;
  return token;
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
  return token;
}

auto Lexer::lex_literal(Token* token) -> std::expected<Token*, Diagnostic> {
  // Lexes a numerical literal, either an integer or a float.
  auto lex_numerical_literal = [&]() -> std::expected<Token*, Diagnostic> {
    token->extend(m_cur);
    while (!source.eof() && lexer_is_digit(peek_next())) {
      token->extend(advance_cursor());
    }

    // Lexing a float.
    if (peek_next() == '.') {
      token->extend(advance_cursor());
      if (!lexer_is_digit(peek_next())) {
        return unexpected_diagnostic_from(
          DiagnosticKind::Critical,
          { "character after radix point is not a digit, found: '" + std::string{static_cast<char>(peek_next())} + "'" },
          {}, //! no suggestions for now.
          CodeLocation(file_name, m_line, m_col)
        );
      }
      
      while (!source.eof() && lexer_is_digit(peek_next())) {
        token->extend(advance_cursor());
      }

      token->kind = TokenKind::LiteralFloat; // Float literal.
      return token;
    }
        
    //! Check for invalid integer literal.
    //!Cases like:
    //!
    //! let gravity: int = 34r; //! NOT ALLOWED.
    //!
    //! Those cases will be tokenized as: [LiteralInt, Identifier].
    //! Since wombat is a lazy-lexed language, the PARSER will catch those errors.
    //! 
    token->kind = TokenKind::LiteralInt;
    return token;
  };

  //! Lexes a string literal.
  //! Handles incorrect syntax-errors like:
  //!
  //! ` let msg: String = "hello!"hi!"; // unterminated string literal. ` 
  //!
  auto lex_string_literal = [&]() -> std::expected<Token*, Diagnostic> {
    //! '"' was processed, so we consume it.
    token->extend(m_cur);

    while (!source.eof()) {
        char next_char = peek_next();

        //! Allow escape characters, for example:
        //!
        //! ` let message: String = "hello, my\"name is\"daniel";
        //!
        if (next_char == '\\') {
          token->extend(advance_cursor());

          if (source.eof()) {
            return unexpected_diagnostic_from(
              DiagnosticKind::Critical,
              { "string ends with an incomplete escape sequence" },
              {},
              CodeLocation(file_name, m_line, m_col)
            );
          }

          char escaped_char = advance_cursor();
          
          //! Wombat allows multiple escape characters in string-literals. 
          if (escaped_char != 'n'  && 
              escaped_char != '\\' && 
              escaped_char != '"'
            ) {
              return unexpected_diagnostic_from(
                DiagnosticKind::Critical,
                { "invalid escape sequence: '\\" + std::string(1, escaped_char) + "'" },
                {},
                CodeLocation(file_name, m_line, m_col)
              );
          }
          
          token->extend(escaped_char);
          continue;
        }

        if (next_char == '"') {
          token->extend(advance_cursor());
          token->kind = TokenKind::LiteralString;
          return token;
        }

        token->extend(advance_cursor());
    }
    
    return unexpected_diagnostic_from(
        DiagnosticKind::Critical,
        { "unterminated string literal" },
        {},
        CodeLocation(file_name, m_line, m_col)
    );
  };

  // Lexes a char literal (e.g. 'a' or '\n').
  auto lex_char_literal = [&]() -> std::expected<Token*, Diagnostic> {
    advance_cursor();
    if (peek_next() == '\'') {
      advance_cursor();
      token->kind = TokenKind::LiteralChar;
      token->value = "";
      return token;
    }
    
    token->extend(advance_cursor()); // Get the character

    if (peek_next() == '\'') {
      advance_cursor();
      token->kind = TokenKind::LiteralChar;
      return token;
    } else {
      return unexpected_diagnostic_from(
        DiagnosticKind::Critical,
        { "unterminated char literal" },
        { Suggestion("try closing " + token->value + " with '") },
        CodeLocation(file_name, m_line, m_col)
      );
    }
  };

  //! If m_cur is not a opening [string/char] literal is it necessarily a digit. 
  if (m_cur == '"')  return lex_string_literal();
  if (m_cur == '\'') return lex_char_literal();
  return lex_numerical_literal();
}


auto Lexer::lexer_next_token(TokenStream* token_stream, Token* token) -> std::expected<Token*, Diagnostic> {
  token->clean();
  skip_whitespaces();
  advance_cursor();

  if(source.eof()) lex_eof(token);
  else if(m_cur == '/' && peek_next() == '/') lex_line_comment(token);
  else if(lexer_is_alpha(m_cur)) lex_word(token);
  else if(lexer_is_digit(m_cur) || m_cur == '"') lex_literal(token);
  else if(lexer_is_symbol(m_cur)) lex_symbol(token);
  else { 
    lex_foreign(token, m_cur); 
  };

  auto res = token_stream->push_token(*token);
  return token;
}

auto Lexer::lex_source() -> TokenStream {
  Token cur_token;
  TokenStream token_stream{};

  lexer_next_token(&token_stream, &cur_token);
  while(!cur_token.compare_kind(TokenKind::Eof)) {
    lexer_next_token(&token_stream, &cur_token);
  }

  return token_stream;
}

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
      case TokenKind::LiteralInt:    
        return "Literal_INTEGER";
      case TokenKind::LiteralFloat:    
        return "Literal_FLOAT";
      case TokenKind::LiteralString:    
        return "Literal_STRING";
      case TokenKind::LiteralChar:    
        return "Literal_CHAR";
      case TokenKind::Identifier:
        return "Identifier";
      case TokenKind::Keyword:
        return "Keyword";
      case TokenKind::Readable:
        return "Readable[Maybe]";
      case TokenKind::Whitespace:      
        return "Whitespace";
      case TokenKind::LineComment:
        return "Single_Line_Comment";
  }
  return "Foreign_Token";
}