#include <iostream>
#include "common.hpp"
#include "token.hpp"

using Tokenizer::Token;
using Tokenizer::TokenKind;
using Tokenizer::LiteralKind;
using Tokenizer::Keyword;
using Tokenizer::BinOpKind;
using Tokenizer::UnOpKind;
using Tokenizer::AssignOp;
using Tokenizer::BooleanKind;

Option<Keyword> Tokenizer::keyword_from_token(const std::string& lexeme) {
  if(lexeme == "import")     return Keyword::Import;
  if(lexeme == "fn")     return Keyword::Fn;
  if(lexeme == "end")    return Keyword::End;
  if(lexeme == "return") return Keyword::Return;
  if(lexeme == "if")     return Keyword::If;
  if(lexeme == "else")   return Keyword::Else; 
  if(lexeme == "let")    return Keyword::Let;
  if(lexeme == "mut")    return Keyword::Mut;
  if(lexeme == "loop")   return Keyword::Loop;
  if(lexeme == "break")  return Keyword::Break;
  if(lexeme == "with")   return Keyword::With;
  if(lexeme == "and")    return Keyword::And;
  if(lexeme == "or")     return Keyword::Or;
  if(lexeme == "not")    return Keyword::Not;
  if(lexeme == "ptr")    return Keyword::Ptr;
  return std::nullopt;
}

Option<LiteralKind> Tokenizer::literal_kind_from_token(const TokenKind& kind) {
  switch (kind) {
    case TokenKind::LiteralNum:     return LiteralKind::Int;
    case TokenKind::LiteralFloat:   return LiteralKind::Float;
    case TokenKind::LiteralChar:    return LiteralKind::Char;
    case TokenKind::LiteralString:  return LiteralKind::Str;
    case TokenKind::LiteralBoolean: return LiteralKind::Bool;
    default: 
      return std::nullopt; 
  }
}

Option<BinOpKind> Tokenizer::bin_op_from_token(const Token& tok) {
  switch (tok.kind) {
    case TokenKind::Plus:         return BinOpKind::Add;
    case TokenKind::Minus:        return BinOpKind::Sub;
    case TokenKind::Star:         return BinOpKind::Mul;
    case TokenKind::Slash:        return BinOpKind::Div;
    case TokenKind::DoubleSlash:  return BinOpKind::FlooredDiv;
    case TokenKind::Precent:      return BinOpKind::Mod;
    case TokenKind::DoubleStar:   return BinOpKind::Pow;
    case TokenKind::Ampersand:    return BinOpKind::BitAnd;
    case TokenKind::Pipe:         return BinOpKind::BitOr;
    case TokenKind::Hat:          return BinOpKind::BitXor;
    case TokenKind::ShiftLeft:    return BinOpKind::Shl;
    case TokenKind::ShiftRight:   return BinOpKind::Shr;  
    case TokenKind::Lt:           return BinOpKind::Lt;
    case TokenKind::Gt:           return BinOpKind::Gt;
    case TokenKind::Le:           return BinOpKind::Le;
    case TokenKind::Ge:           return BinOpKind::Ge;
    case TokenKind::DoubleEq:     return BinOpKind::Eq;
    case TokenKind::NotEq:        return BinOpKind::NotEq;
    default: {
      auto keyword_match = keyword_from_token(tok.value);

      if(!keyword_match.has_value()) {
        return std::nullopt;
      }

      switch (keyword_match.value()) {
        case Keyword::And:  return BinOpKind::And;
        case Keyword::Or:   return BinOpKind::Or;
        default: 
          return std::nullopt;
      }
    }
  }
}

Option<UnOpKind> Tokenizer::un_op_from_token(const Token& tok) {
  switch (tok.kind) {
    case TokenKind::Tilde:  return UnOpKind::BitNot;
    case TokenKind::Minus:  return UnOpKind::Neg;
    default: {
      if(tok.match_keyword(Keyword::Not)) {
        return UnOpKind::Not;
      } else {
        return std::nullopt;
      }
    }
  }
}

Option<AssignOp> Tokenizer::assign_op_from_token(const Token& tok) {
  switch (tok.kind) {
    case TokenKind::Eq: return AssignOp::Eq;
    case TokenKind::StarAssign: return AssignOp::Mul;
    case TokenKind::SlashAssign: return AssignOp::Div;
    case TokenKind::PrecentAssign: return AssignOp::Mod;
    case TokenKind::PlusAssign: return AssignOp::Plus;
    case TokenKind::MinusAssign: return AssignOp::Minus;
    case TokenKind::ShlAssign: return AssignOp::Shl;
    case TokenKind::ShrAssign: return AssignOp::Shr;
    case TokenKind::AmpersandAssign: return AssignOp::And;
    case TokenKind::PipeAssign: return AssignOp::Or;
    case TokenKind::HatAssign: return AssignOp::Xor;
    default: 
      return std::nullopt;
  }
}

Option<BooleanKind> Tokenizer::bool_from_token(const Token& tok) {
  if(tok.value == "true")  return BooleanKind::True;
  if(tok.value == "false") return BooleanKind::False;
  return std::nullopt;
}

std::string Tokenizer::meaning_from_kind(const TokenKind& kind) {
  switch (kind) {
      case TokenKind::OpenParen: return "Open_Paren";
      case TokenKind::CloseParen: return "Close_Paren";
      case TokenKind::OpenBracket: return "Open_Bracket";
      case TokenKind::CloseBracket: return "Close_Bracket";
      case TokenKind::OpenCurly: return "Open_Curly";
      case TokenKind::CloseCurly: return "Close_Curly";
      case TokenKind::OpenAngle: return "Open_Angle";
      case TokenKind::CloseAngle: return "Close_Angle";
      case TokenKind::Lt: return "Less_Than";
      case TokenKind::Gt: return "Greater_Than";
      case TokenKind::ReturnSymbol: return "Arrow";
      case TokenKind::DoubleStar: return "Power_Operator";
      case TokenKind::Minus: return "Minus_Operator";
      case TokenKind::Plus: return "Plus_Operator";
      case TokenKind::Star: return "Multiplication_Operator";
      case TokenKind::Slash: return "Division_Operator";
      case TokenKind::Precent: return "Modulus_Operator";
      case TokenKind::DoubleEq: return "Equality_Operator";
      case TokenKind::NotEq: return "Not_Equality_Operator";
      case TokenKind::Eq: return "Equals_Assignment";
      case TokenKind::Le: return "Less_Than_Or_Equal_To";
      case TokenKind::Ge: return "Greater_Than_Or_Equal_To";
      case TokenKind::Eof: return "End_Of_File";
      case TokenKind::Colon: return "Colon";
      case TokenKind::SemiColon: return "Semi_Colon";
      case TokenKind::Comma: return "Comma";
      case TokenKind::Dot: return "Dot";
      case TokenKind::LiteralNum: return "Literal_NUMBER";
      case TokenKind::LiteralFloat: return "Literal_FLOAT";
      case TokenKind::LiteralString: return "Literal_STRING";
      case TokenKind::LiteralChar: return "Literal_CHAR";
      case TokenKind::LiteralBoolean: return "Literal_BOOLEAN";
      case TokenKind::Identifier: return "Identifier";
      case TokenKind::Keyword: return "Keyword";
      case TokenKind::Readable: return "Readable";
      case TokenKind::Whitespace: return "Whitespace";
      case TokenKind::LineComment: return "Single_Line_Comment";
      case TokenKind::Bang: return "Bang";
      case TokenKind::Ampersand: return "Ampersand";
      case TokenKind::Pipe: return "Pipe";
      case TokenKind::Hat: return "Hat";
      case TokenKind::Tilde: return "Tilde";
      case TokenKind::DoubleSlash: return "Floor_Division_Operator";
      case TokenKind::ShiftRight: return "Shift_Right";
      case TokenKind::ShiftLeft: return "Shift_Left";
      case TokenKind::StarAssign: return "Multiply_Assign";
      case TokenKind::SlashAssign: return "Divide_Assign";
      case TokenKind::PrecentAssign: return "Modulo_Assign";
      case TokenKind::PlusAssign: return "Plus_Assign";
      case TokenKind::MinusAssign: return "Minus_Assign";
      case TokenKind::ShlAssign: return "Shift_Left_Assign";
      case TokenKind::ShrAssign: return "Shift_Right_Assign";
      case TokenKind::AmpersandAssign: return "Bitwise_And_Assign";
      case TokenKind::PipeAssign: return "Bitwise_Or_Assign";
      case TokenKind::HatAssign: return "Bitwise_Xor_Assign";
      case TokenKind::Foreign: return "Foreign_Token";
      case TokenKind::None: return "None_Token";
      default: {
          ASSERT(false, "unknown_TOKEN");
      }
  }
}

std::string Tokenizer::meaning_from_literal_kind(
  const LiteralKind& kind
) {
  switch (kind) {
    case LiteralKind::Int:    return "Int_LITERAL";
    case LiteralKind::Float:  return "Float_LITERAL";
    case LiteralKind::Char:   return "Char_LITERAL";
    case LiteralKind::Str:    return "Str_LITERAL";
    case LiteralKind::Bool:   return "Bool_LITERAL";
    default: {
      ASSERT(false, "unknown_LITERAL");
    } 
  }
}

std::string Tokenizer::meaning_from_bin_op_kind(const BinOpKind& kind) {
  switch (kind) {
      case BinOpKind::Add:        return "Addition";
      case BinOpKind::Sub:        return "Subtraction";
      case BinOpKind::Mul:        return "Multiplication";
      case BinOpKind::Pow:        return "Exponentiation";
      case BinOpKind::Div:        return "Division";
      case BinOpKind::FlooredDiv: return "Floor_Division";
      case BinOpKind::Mod:        return "Modulus";
      case BinOpKind::And:        return "Logical_And";
      case BinOpKind::Or:         return "Logical_Or";
      case BinOpKind::BitXor:     return "Bitwise_XOR";
      case BinOpKind::BitAnd:     return "Bitwise_AND";
      case BinOpKind::BitOr:      return "Bitwise_OR";
      case BinOpKind::Shl:        return "Shift_Left";
      case BinOpKind::Shr:        return "Shift_Right";
      case BinOpKind::Eq:         return "Equality";
      case BinOpKind::Lt:         return "Less_Than";
      case BinOpKind::Le:         return "Less_Than_Equal_To";
      case BinOpKind::NotEq:      return "Not_Equal_To";
      case BinOpKind::Ge:         return "Greater_Than_Equal_To";
      case BinOpKind::Gt:         return "Greater_Than";
      default: {
        ASSERT(false, "unknown_BinOp");
      }
  }
}

std::string Tokenizer::meaning_from_un_op_kind(const UnOpKind& kind) {
  switch (kind) {
      case UnOpKind::Neg:    return "Negation";
      case UnOpKind::Not:    return "Logical_Not";
      case UnOpKind::BitNot: return "Bitwise_Not";
      default: {
        ASSERT(false, "unknown_UnOp");
      } 
  }
}

std::string Tokenizer::meaning_from_assign_op_kind(const AssignOp& kind) {
  switch (kind)
  {
    case AssignOp::Eq: return "Eq_Assign";
    case AssignOp::Mul: return "Mul_Assign";
    case AssignOp::Div: return "Div_Assign";
    case AssignOp::Mod: return "Mod_Assign";
    case AssignOp::Plus: return "Plus_Assign";
    case AssignOp::Minus: return "Minus_Assign";
    case AssignOp::Shl: return "Shl_Assign"; 
    case AssignOp::Shr: return "Shr_Assign"; 
    case AssignOp::And: return "And_Assign"; 
    case AssignOp::Or: return "Or_Assign";
    case AssignOp::Xor: return "Xor_Assign"; 
    default: {
      ASSERT(false, "unknown_AssignOp");
    }
  }
}

void Token::out() const {
  std::cout << "Token {\n"
            << "  kind: TokenKind::" << meaning_from_kind(kind) << ",\n"
            << "  value: \"" << value << "\",\n"
            << "  loc: {\n"
            << "    line: " << loc.line << ",\n"
            << "    column: " << loc.col << "\n"
            << "  }\n"
            << "}\n";
}