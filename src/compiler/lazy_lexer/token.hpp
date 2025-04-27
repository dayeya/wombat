#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <vector>
#include <string>
#include <variant>
#include <unordered_map>
#include <algorithm>

#include "alias.hpp"
#include "common.hpp"

namespace Tokenizer {

enum class TokenKind: int {
    //!
    //! Literals
    LiteralNum,
    LiteralFloat,
    LiteralChar,
    LiteralString,
    LiteralBoolean, 
    //!
    //! Identifier can be either of two options: user-defined or keyword.
    //!
    Identifier,         // Represents user-defined data, data-types and behavior. e.g variable-name, functions and structs.
    Keyword,            // Words that represent a meaningful behavior/feature in the language. e.g 'let', 'func' and so on and so forth.
    //!
    //! Readables!, wombat offers a core-syntax language feature! 
    //! See [readables] in wombat-docs.
    //!
    //! Briefly, readables make developers code cleaner and readable code!
    //! They operate on a single-argument and they are notated by the '!' symbol.
    //! e.g:
    //! 
    //! let x: int = 8;
    //! let p_x: ref<int> = &x 
    //!
    //! For wombat, this code is *not* readable.
    //! Wombat proposes the following:
    //! 
    //! let x: int = 8; 
    //! let p_x: ref<int> = mem! x;
    //!
    Readable,

    //! Punctuators
    OpenParen,          // Symbol for '('
    CloseParen,         // Symbol for ')'
    OpenBracket,        // Symbol for '['
    CloseBracket,       // Symbol for ']'
    OpenCurly,          // Symbol for '{'
    CloseCurly,         // Symbol for '}'
    OpenAngle,          // Symbol for '>'
    CloseAngle,         // Symbol for '<'
    Colon,              // Symbol for ':'
    SemiColon,          // Symbol for ';'
    Comma,              // Symbol for ','
    Dot,                // Symbol for ','
    Bang,               // Symbol for '!'
    Ampersand,          // Symbol for '&'
    Pipe,               // Symbol for '|'
    Hat,                // Symbol for '^'
    Tilde,              // Symbol for '~'

    Whitespace,         // Sequence of non-meaningful characters like spaces or tabs

    Plus,               // Arithmetic addition symbol,          '+'
    Minus,              // Arithmetic subtraction symbol,       '-'
    Star,               // Arithmetic multiplication symbol,    '*'
    DoubleStar,         // Arithmetic power symbol,             '**'
    Slash,              // Arithmetic division symbol,          '/'
    DoubleSlash,        // Arithmetic floor division,           '//'
    ShiftRight,         // Arithmetic floor division,           '>>'
    ShiftLeft,          // Arithmetic floor division,           '<<'
    Precent,            // Arithmetic modulu-op symbol,         '%'
    Eq,                 // Assignment operator,                 '='
    StarAssign,         // Assignment operator,                 '*='
    SlashAssign,        // Assignment operator,                 '/='
    PrecentAssign,      // Assignment operator,                 '%='     
    PlusAssign,         // Assignment operator,                 '+='
    MinusAssign,        // Assignment operator,                 '-='
    ShlAssign,          // Assignment operator,                 '<<='
    ShrAssign,          // Assignment operator,                 '>>='
    AmpersandAssign,    // Assignment operator,                 '&='
    PipeAssign,         // Assignment operator,                 '|='
    HatAssign,          // Assignment operator,                 '^='
    Lt,                 // Less than symbol,                    '<'
    Gt,                 // Greater than symbol,                 '>'
    ReturnSymbol,       // Return type indicator for functions, '->'
    DoubleEq,           // Equality operator,                   '=='
    NotEq,              // reverse equality operator,           '!='
    Le,                 // Less than or equal to operator,      '<='
    Ge,                 // Greater than or equal to operator,   '>='
    Eof,                // End of file marker

    LineComment,        // Single-line comment, '//'
    Foreign,            // Unknown or invalid token for the language
    None                // Indicator for an empty token
};

enum class Keyword: int {
    // 'import' denotes an import statement.
    Import,
    // 'fn' denotes either function declaration, implementation or a function pointer via ref<fn ...>
    Fn, 
    // 'return' denotes the return value of a function.
    Return,
    // 'end' denotes an end of a scope, either with function bodies or match blocks.
    End,
    // 'let' statement, denotes a declaration of an identifier.
    Let,
    // 'mut' modifier
    Mut,
    // flow-control - if, else.
    // 'if' denotes an if block, 'else' denotes an else block. 
    // note: scopes are denoted by curly braces as opposed to function and match blocks.
    If, 
    Else,
    // flow-control - loops.
    // 'loop' denotes a block using curly braces that will repeat itself.
    // 'stop' - breaks out of a loop.
    // 'with' - introduces a unique resource that bounds only to the scope of the loop.
    Loop,
    Break,
    With,
    // Logical operations.
    And, 
    Or, 
    Not 
};

enum class LiteralKind: int {
    // Literal representing integers. e.g '3'.
    Int, 
    // Literal representing floats. e.g '3.14'.
    Float,
    // Literal representing chars. e.g 'a'.
    Char,
    // Literal representing strings. e.g 'Hello, Wombat!'.
    Str,
    // Boolean
    Bool,
    // Dummy value
    None
};

enum class BooleanKind: int {
    True, False
};

enum class BinOpKind: int {
    // The `+` operator (addition)
    Add,
    // The `-` operator (subtraction)
    Sub,
    // The `*` operator (multiplication)
    Mul,
    // The `**` operator (pow)
    Pow,
    // The `/` operator (division)
    Div,
    // The `//` operator (floor division)
    FlooredDiv,
    // The `%` operator (modulus)
    Mod,
    // The `and` operator (logical and)
    And,
    // The `or` operator (logical or)
    Or,
    // The `^` operator (bitwise xor)
    BitXor,
    // The `&` operator (bitwise and)
    BitAnd,
    // The `|` operator (bitwise or)
    BitOr,
    // The `<<` operator (shift left)
    Shl,
    // The `>>` operator (shift right)
    Shr,
    // The `==` operator (equality)
    Eq,
    // The `<` operator (less than)
    Lt,
    // The `<=` operator (less than or equal to)
    Le,
    // The `!=` operator (not equal to)
    NotEq,
    // The `>=` operator (greater than or equal to)
    Ge,
    // The `>` operator (greater than)
    Gt
};

enum class AssignOp: int {
    // The `=` operator (simple assignment)
    Eq,
    // The `*=` operator (multiply and assign)
    Mul,
    // The `/=` operator (divide and assign)
    Div,
    // The `%=` operator (modulus and assign)
    Mod,
    // The `+=` operator (add and assign)
    Plus,
    // The `-=` operator (subtract and assign)
    Minus,
    // The `<<=` operator (left shift and assign)
    Shl,
    // The `>>=` operator (right shift and assign)
    Shr,
    // The `&=` operator (bitwise and and assign)
    And,
    // The `|=` operator (bitwise or and assign)
    Or,
    // The `^=` operator (bitwise xor and assign)
    Xor
};

enum class UnOpKind: int {
    // Negation. e.g `-2`.
    Neg,
    // Logical not. for example:
    // `loop {
    //     if not found: ...
    // }`
    Not,
    // A bitwise not. marked by '~'.
    BitNot
};

// Converts the value of a token into a keyword.
// Returns an optional wrapping a `Tokenizer::Keyword` or an `std::nullopt`.
Option<Keyword> keyword_from_token(const std::string& lexeme);

// Converts a `Tokenizer::TokenKind` into a `Tokenizer::LiteralKind`.
// Returns an optional wrapping a `Tokenizer::LiteralKind` or an `std::nullopt`.
Option<LiteralKind> literal_kind_from_token(const TokenKind& kind);

// Converts a `Tokenizer::TokenKind` into its string representation.
std::string meaning_from_kind(const TokenKind& kind);

// Converts a `Tokenizer::LiteralKind` into its string representation.
std::string meaning_from_literal_kind(const LiteralKind& kind);

// Converts a `Tokenizer::BinOpKind` into its string representation.
std::string meaning_from_bin_op_kind(const BinOpKind& kind);

// Converts a `Tokenizer::UnOpKind` into its string representation.
std::string meaning_from_un_op_kind(const UnOpKind& kind);

// Converts a `Tokenizer::AssignOp` into its string representation.
std::string meaning_from_assign_op_kind(const AssignOp& kind);

/// @brief `Tokenizer::Location`
/// Is a struct wrapping the line and a column of a certain context within the source code.  
struct Location {
    int line;
    int col;

    Location(int l, int c) : line(l), col(c) {}  // Constructor

    // A getter for a singularity location `{ line: 0, col: 0 }`
    // Think of it as a dummy node for error parsing.
    static inline auto Singularity() -> Location {
        return { 0, 0 };
    }
};

/// @brief `Tokenizer::Token`
/// Is a struct that stores information about a specific lexeme found within the source code.
struct Token {
    TokenKind kind;
    std::string value;
    Location loc;

    Token() : kind{TokenKind::None}, value{}, loc{Location::Singularity()} {}

    Token(TokenKind k, std::string v, int l, int c)
        : kind{k}, value{std::move(v)}, loc{l, c} {}

    // Pretty-prints the tokens data.
    void out() const;

    // Resets the token.
    void clean() {
        value.clear();
        kind = TokenKind::None;
        loc = Location::Singularity();
    }

    // Appends a character to the token's value.
    void extend(char ch) {
        value += ch;
    }

    // Checks if the token matches a specific `Tokenizer::TokenKind`.
    // A single-match closure is used a lot within the compiler frontend, so we created a seperate function.
    bool match_kind(TokenKind k) const {
        return kind == k;
    }

    // Checks if the token's kind matches any of the provided token kinds.
    template<typename... Kinds>
    bool matches_any(Kinds... kinds) const {
        return ((kind == kinds) || ...);
    }
    
    // Checks if the token represents a keyword.
    template<typename... Kw>
    bool match_keyword(Kw... keywords) const {
        if(!match_kind(TokenKind::Keyword)) return false;
        auto keyword_match = Tokenizer::keyword_from_token(value);
        return keyword_match.has_value() && ((keyword_match.value() == keywords) || ...);
    }

    // Populates the token with new values.
    void fill_with(std::string v, TokenKind k) {
        kind = k;
        value = std::move(v);
    }

    // Updates the tokens location in the source code.
    void update_location(int line, int col) {
        loc = {line, col};
    }
};

/// @brief `Tokenizer::LazyTokenStream`
/// Is a struct that stores the token-stream buffer, meant for travesing and parsing.
struct LazyTokenStream {
    int cur = -1;
    std::vector<Token> m_tokens;

    LazyTokenStream() : m_tokens() {}
    ~LazyTokenStream() = default;

    // Resets the cursors position.
    void reset() {
        cur = -1;
    }

    // Size casting, from `std::size_t` to a primitive int.
    inline int self_size() const {
        return static_cast<int>(m_tokens.size());
    }

    // Checks wether the token stream holds an End-of-file token.
    inline bool reached_eof(int k) const {
        // means we want to check the first token. 
        if (k == -1) {
            k++;
        }
        return (
            !m_tokens.empty() &&
            m_tokens.at(k).match_kind(TokenKind::Eof)
        );
    }

    // Checks if the stream reached its end.
    inline bool has_next() const {
        return !reached_eof(cur);
    }

    // Pushes a token into the buffer.
    void feed(const Token& token) {
        cur++;
        m_tokens.push_back(token);
    }

    // Consumes a token from the stream.
    Option<Token> eat_one_token() {
        if(!has_next()) {
            return std::nullopt;
        } else {
            return std::make_optional<Token>(m_tokens.at(++cur));
        }
    }

    void dump() const {
        for(const auto& tok : m_tokens) tok.out();
    }
};

// Converts a 'Tokenizer::TokenKind` into a `Tokenizer::BinOpKind`.
Option<BinOpKind> bin_op_from_token(const Token& tok);

// Converts a 'Tokenizer::TokenKind` into a `Tokenizer::UnOpKind`.
Option<UnOpKind> un_op_from_token(const Token& tok); 

// Converts a 'Tokenizer::TokenKind` into a `Tokenizer::AssignOp`.
Option<AssignOp> assign_op_from_token(const Token& tok);

// Converts a 'Tokenizer::TokenKind` into a `Tokenizer::BooleanKind`.
Option<BooleanKind> bool_from_token(const Token& tok);

};

#endif // TOKEN_HPP_