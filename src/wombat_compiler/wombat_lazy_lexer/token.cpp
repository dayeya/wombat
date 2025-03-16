#include <iostream>

#include "token.hpp"

constexpr std::string kind_to_str(const TokenKind& kind) {
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
    case TokenKind::Star:
      return "Multiplication_Operator";
    case TokenKind::Div:            
      return "Division_Operator";
    case TokenKind::Precent:            
      return "Modulus_Operator";
    case TokenKind::DoubleEq:  
      return "Equality_Operator";
    case TokenKind::NotEq:  
      return "Not_Equality_Operator";
    case TokenKind::Eq: 
      return "Equals_Assignment";
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
      return "Comma";   
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
    default:
      return "Foreign_Token";
  }
}

void Token::out() const {
  std::cout << "Token {" << "\n"
            << "  " << "kind: TokenKind::" << kind_to_str(kind) << "," << "\n"
            << "  " << "value: \"" << value << "\"," << "\n"
            << "  " << "loc: {" << "\n"
            << "    " << "line: " << loc.line << "\n"
            << "    " << "column: " << loc.col << "\n"
            << "  " << "}" << "\n"
            << "}" << "\n";
}