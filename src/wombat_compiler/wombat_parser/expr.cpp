#include "expr.hpp"

LiteralKind specify_literal_kind(const TokenKind& kind) {
    switch(kind) {
        case TokenKind::LiteralNum: 
            return LiteralKind::Int;
        case TokenKind::LiteralFloat: 
            return LiteralKind::Float;
        case TokenKind::LiteralChar: 
            return LiteralKind::Char;
        case TokenKind::LiteralString: 
            return LiteralKind::Str;
        case TokenKind::LiteralBoolean: 
            return LiteralKind::Bool;
        default:
            WOMBAT_ASSERT(false, "undefined token for conversion");
    }
}

BinaryOperator specify_bin_op(const TokenKind& kind) {
    switch(kind) {
        case TokenKind::Plus: 
            return BinaryOperator::Add;
        case TokenKind::Minus: 
            return BinaryOperator::Sub;
        case TokenKind::Star: 
            return BinaryOperator::Mul;
        case TokenKind::Div: 
            return BinaryOperator::Div;
        case TokenKind::Precent:
            return BinaryOperator::Mod;
        default:
            WOMBAT_ASSERT(false, "undefined token for conversion");
    }
}

std::string lit_kind_to_str(const LiteralKind& kind) {
    switch (kind) {
        case LiteralKind::Int:
            return "Integer";
        case LiteralKind::Float:  
            return "Float";
        case LiteralKind::Char:     
            return "Char";
        case LiteralKind::Str:  
            return "String";
        case LiteralKind::Bool:       
            return "Boolean";
        default:
            WOMBAT_ASSERT(false, "undefined literal kind");
    }
}

std::string bin_op_kind_to_str(const BinaryOperator& kind) {
    switch (kind) {
        case BinaryOperator::Add:
            return "Addition";
        case BinaryOperator::Sub:  
            return "Subtraction";
        case BinaryOperator::Mul:     
            return "Multiplication";
        case BinaryOperator::Div:  
            return "Division";
        case BinaryOperator::Mod:    
            return "Modulus";
        default:
            WOMBAT_ASSERT(false, "undefined binary operator kind");
    }
}