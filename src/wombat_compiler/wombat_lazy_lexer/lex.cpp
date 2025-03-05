#include <iostream>
#include <optional>
#include <expected>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>

#include "cutil.hpp"
#include "lex.hpp"

void Lexer::lex_eof(unique_ptr<Token>& token) {
  token->kind = TokenKind::Eof;
  token->pos = std::make_pair(m_cursor.current_line, m_cursor.current_col);
}

void Lexer::lex_foreign(unique_ptr<Token>& token, char ch) {
  token->extend(ch);
  token->kind = TokenKind::Foreign;
  token->pos = std::make_pair(m_cursor.current_line, m_cursor.current_col);
}

void Lexer::lex_line_comment(unique_ptr<Token>& token) {
  //! Eat double forward slash.
  advance_cursor();
  advance_cursor();

  //! Until we reached end of line we stop.
  while(!m_cursor.reached_eof() && !m_cursor.reached_new_line()) {
    auto _ = advance_cursor();
  }
}

void Lexer::lex_word(unique_ptr<Token>& token) {
  token->set_pos(m_cursor.current_line, m_cursor.current_col);
  token->extend(m_cursor.current);  

  while(
    !m_cursor.reached_eof() &&
    CharUtils::is_alnum(m_cursor.peek_next()) || m_cursor.peek_next() == '_'
  ) {
    token->extend(advance_cursor());
  }

  //! Assuming that the current token is a readable, if it is followed by a '!';
  if(m_cursor.peek_next() == '!') {
    token->extend(advance_cursor());
    token->kind = TokenKind::Readable;
    return;
  }

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
  } else {
    token->kind = TokenKind::Identifier;
  }
}

void Lexer::lex_symbol(unique_ptr<Token>& token) {
  token->set_pos(m_cursor.current_line, m_cursor.current_col);

  switch (m_cursor.current) {
    case '(': token->fill_with("(", TokenKind::OpenParen);    break;
    case ')': token->fill_with(")", TokenKind::CloseParen);   break;
    case '{': token->fill_with("{", TokenKind::OpenCurly);    break;
    case '}': token->fill_with("}", TokenKind::CloseCurly);   break;
    case '[': token->fill_with("[", TokenKind::OpenBracket);  break;
    case ']': token->fill_with("]", TokenKind::CloseBracket); break;
    case ':': token->fill_with(":", TokenKind::Colon);        break;
    case ';': token->fill_with(";", TokenKind::SemiColon);    break;
    case ',': token->fill_with(",", TokenKind::Comma);        break;
    case '.': token->fill_with(".", TokenKind::Dot);          break;
    case '+': token->fill_with("+", TokenKind::Plus);         break;
    default: break;
  }

  //! If token was assigned with a value in the above switch statement, we exit.
  if(token->kind != TokenKind::None) {
    return;
  }

  switch (m_cursor.current)
  {
    case '<':
      if (m_cursor.peek_next() == '=') {
        token->fill_with("<=", TokenKind::Le);
        advance_cursor();
      } else {
        token->fill_with("<", TokenKind::OpenAngle);
      }
      break;
    case '>':
      if (m_cursor.peek_next() == '=') {
        token->fill_with(">=", TokenKind::Ge);
        advance_cursor();
      } else {
        token->fill_with(">", TokenKind::CloseAngle);
      }
      break;
    case '-':
      if (m_cursor.peek_next() == '>') {
        token->fill_with("->", TokenKind::ReturnSymbol);
        advance_cursor();
      } else {
        token->fill_with("-", TokenKind::Minus);
      }
      break;
    case '=':
      if (m_cursor.peek_next() == '=') {
        token->fill_with("==", TokenKind::DoubleEq);
        advance_cursor();
      } else {
        token->fill_with("=", TokenKind::Eq);
      }
      break;
    default:
      token->fill_with(
        std::string{m_cursor.current}, 
        TokenKind::Foreign
      );
      break;
  }
}

void Lexer::lex_literal(unique_ptr<Token>& token) {
  // Lexes a numerical literal, either an integer or a float.
  auto lex_numerical_literal = [&]() -> void {
    token->extend(m_cursor.current);
    while (!m_cursor.reached_eof() && CharUtils::is_digit(m_cursor.peek_next())) {
      token->extend(advance_cursor());
    }
    token->kind = TokenKind::LiteralNum;
  };

  //! Lexes a string literal.
  auto lex_string_literal = [&]() {
    //! '"' was processed, so we consume it.
    token->extend(m_cursor.current);

    while (!m_cursor.reached_eof()) {
        char next_char = m_cursor.peek_next();

        if (next_char == '\\') {
          token->extend(advance_cursor());

          char escaped_char = advance_cursor();

          if (
              escaped_char != 'n'  && 
              escaped_char != '\\' && 
              escaped_char != '"'
            ) {
              std::string message("unexpected escape sequence: "); 
              message += "'\\" + std::string(1, escaped_char) + "'";

              auto region = Region {
                m_cursor.file_name,
                m_cursor.current_line,
                m_cursor.current_col,
                m_cursor.one_lined_region(),
              };

              std::vector<Label> labels{Label{
                "invalid char", 
                std::vector<Region>{region}
              }}; 

              register_critical_diagnostic_pretty(message, "", labels);
          }
          
          token->extend(escaped_char);
          continue;
        }

        if (next_char == '"') {
          token->extend(advance_cursor());
          token->kind = TokenKind::LiteralString;
          return;
        }

        token->extend(advance_cursor());
    }

    std::string message = "unterminated string literal";

    auto region = Region {
      m_cursor.file_name,
      m_cursor.current_line,
      token->pos.second,
      m_cursor.one_lined_region(token->pos.first),
    };

    std::vector<Label> labels{Label{
      "string starts here but is not terminated",
      std::vector<Region>{region}
    }}; 

    register_critical_diagnostic_pretty(message, "", labels);
  };

  // Lexes a char literal (e.g. 'a' or '\n').
  auto lex_char_literal = [&]() {
    advance_cursor();

    if (m_cursor.current == '\'') {
      advance_cursor();
      token->kind = TokenKind::LiteralChar;
      token->value = "";
      return;
    }
    
    token->extend(m_cursor.current);

    if (m_cursor.peek_next() == '\'') {
      advance_cursor();
      token->kind = TokenKind::LiteralChar;
    } else {
      auto region = Region {
        m_cursor.file_name,
        m_cursor.current_line,
        token->pos.second,
        m_cursor.one_lined_region(token->pos.first),
      };
      
      std::vector<Label> labels{
        Label{"", std::vector<Region>{region}}
      }; 

      register_critical_diagnostic_pretty("unterminated char literal", "close the literal with `\'`", labels);
    }
  };

  token->pos = std::make_pair(m_cursor.current_line, m_cursor.current_col);

  //! If m_cursor.current is not a opening [string/char] literal is it necessarily a digit. 
  if (m_cursor.current == '"')  lex_string_literal();
  else if (m_cursor.current == '\'') lex_char_literal();
  else {
    lex_numerical_literal();
  }
}

void Lexer::next_token(LazyTokenStream& token_stream) {
  m_cursor.skip_whitespace();
  advance_cursor();

  if(m_cursor.reached_new_line()) return;

  if(
    m_cursor.current == '/' && 
    m_cursor.peek_next() == '/'
  ) {
    lex_line_comment(token_stream.m_current_token);
    return;
  }

  if(m_cursor.reached_eof()) {
    lex_eof(token_stream.m_current_token);
  }
  else if(
    CharUtils::is_alpha(m_cursor.current) || 
    m_cursor.current == '_'
  ) {
    lex_word(token_stream.m_current_token);
  }
  else if(
    CharUtils::is_digit(m_cursor.current) || 
    m_cursor.current == '"' || 
    m_cursor.current == '\''
  ) {
    lex_literal(token_stream.m_current_token);
  }
  else if(CharUtils::is_symbol(m_cursor.current)) {
    lex_symbol(token_stream.m_current_token);
  }
  else { 
    lex_foreign(token_stream.m_current_token, m_cursor.current); 
  };
  token_stream.advance_with_token(std::move(token_stream.m_current_token));
}

bool Lexer::open_and_populate_cursor() {
  std::ifstream ifs(m_cursor.file_name, std::ios::in);

  if(!ifs.is_open()) {  
    register_critical_diagnostic_short("could not open: " + m_cursor.file_name, strerror(errno));
    return false;
  }
  
  std::string line;
  while (std::getline(ifs, line))
  {
    m_cursor.source.push_back(line);
  }
  m_cursor.total_lines = m_cursor.source.size();

  ifs.close();
  return true;
}

auto Lexer::lex_source() -> LazyTokenStream {
  LazyTokenStream token_stream{};

  if(!open_and_populate_cursor()) {
    return token_stream;
  }

  do {
    next_token(token_stream);
  } while(!token_stream.reached_end_of_stream());

  return token_stream;
}