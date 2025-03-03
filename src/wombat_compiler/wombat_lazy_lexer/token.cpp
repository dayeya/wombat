#include <iostream>

#include "token.hpp"

std::string kind_to_str(const TokenKind& kind) {
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
    case TokenKind::Comma:
      return "Semi_Colon";   
    case TokenKind::Dot:
      return "Dot";   
    case TokenKind::LiteralNum:    
      return "Literal_NUMBER";
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

void Token::token_to_str() const {
  std::cout << "Token {" << "\n"
            << "  " << "kind: TokenKind::" << kind_to_str(kind) << "," << "\n"
            << "  " << "value: \"" << value << "\"," << "\n"
            << "  " << "pos: {" << "\n"
            << "    " << "line: " << pos.first << "\n"
            << "    " << "column: " << pos.second << "\n"
            << "  " << "}" << "\n"
            << "}" << "\n";
}

void LazyTokenStream::advance_with_token(std::unique_ptr<Token> token) {
  m_tokens.push_back(std::move(token)); 
  m_current_token = std::make_unique<Token>();
}