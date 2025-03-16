#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <vector>
#include <string>
#include <algorithm>

#include "wtypes.hpp"

/**
 * @enum TokenKind
 * @brief Defines all possible tokens within the lexical boundaries of Wombat.
 */
enum class TokenKind {
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

    Whitespace,         // Sequence of non-meaningful characters like spaces or tabs

    Plus,               // Arithmetic addition symbol,          '+'
    Minus,              // Arithmetic subtraction symbol,       '-'
    Star,               // Arithmetic multiplication symbol,    '*'
    Div,                // Arithmetic division symbol,          '/'
    Precent,            // Arithmetic modulu-op symbol,         '%'
    Lt,                 // Less than symbol,                    '<'
    Gt,                 // Greater than symbol,                 '>'
    ReturnSymbol,       // Return type indicator for functions, '->'
    DoubleEq,           // Equality operator,                   '=='
    NotEq,              // Equality operator,                   '!='
    Eq,                 // Assignment operator,                 '='
    Le,                 // Less than or equal to operator,      '<='
    Ge,                 // Greater than or equal to operator,   '>='
    Eof,                // End of file marker

    LineComment,        // Single-line comment, '//'
    Foreign,            // Unknown or invalid token for the language
    None                // Indicator for an empty token
};

enum class Keyword {
    // 'fn' denotes either function declaration, implementation or a function pointer via ref<fn ...>
    Fn, 
    // 'return' denotes the return value of a function.
    Return,
    // 'end' denotes an end of a scope, either with function bodies or match blocks.
    End,
    // 'let' statement, denotes a declaration of an identifier.
    Let,
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
    Stop,
    With
};

enum class LiteralKind {
    // Literal representing integers. e.g '3'.
    Int, 
    // Literal representing floats. e.g '3.14'.
    Float,
    // Literal representing chars. e.g 'a'.
    Char,
    // Literal representing strings. e.g 'Hello, Wombat!'.
    Str,
    // Boolean
    Bool
};

enum class BinaryOperator {
    // Addition
    Add, 
    // Subtraction
    Sub,
    // Multiplication
    Mul, 
    // Division
    Div,
    // Modulus
    Mod
};

/**
 * @brief Converts a TokenKind to its string representation.
 * @param kind The token kind.
 * @return The corresponding string.
 */
constexpr std::string kind_to_str(const TokenKind& kind);

struct Location {
    int line;
    int col;

    Location(int l, int c) 
        : line(l), col(c) {}
};

// A singularity point, represents the start position of any file.
const Location SINGULARITY = Location(0, 0);

/**
 * @struct Token
 * @brief Represents a token identified by the lexer.
 */
struct Token {
    TokenKind kind;
    std::string value;
    Location loc;

    Token()
        : kind{TokenKind::None}, value{""}, loc{SINGULARITY} {}

    Token(TokenKind k, std::string v, int l, int c)
        : kind{k}, value{std::move(v)}, loc(l, c) {}

    // Pretty-prints the token to 'stdout'.
    void out() const;

    // Resets the token to an empty state.
    void clean() {
        value.clear();
        kind = TokenKind::None;
        loc = SINGULARITY;
    }

    // Appends a character to the token's value.
    void extend(char ch) {
        value += ch;
    }

    // Compares kind to another 'TokenKind'. 
    bool match(TokenKind k) const {
        return kind == k;
    }

    // Matches every kinds in 'kinds' with the inner kind.
    template<typename... Kind>
    bool matches_any(Kind... kinds) const {
        std::vector<TokenKind> vk = {kinds...};
        return std::find(vk.begin(), vk.end(), kind) != vk.end();
    }

    void fill_with(
        std::string v, 
        TokenKind k, 
        int line = -1, 
        int col = -1
    ) {
        value = std::move(v);
        kind = k;
        loc.line = line;
        loc.col = col;
    }

    void set_value(std::string&& v) {
        value = std::move(v);
    }

    void set_kind(TokenKind k) {
        kind = k;
    }

    void set_loc(int line, int col) {
        loc.line = line;
        loc.col = col;
    }
};

//! Token stream state.
enum class TState {
    NotYet,
    Traversing,
    Ended
};

/**
 * @struct LazyTokenStream
 * @brief A stream of tokens supporting lazy evaluation.
 */
struct LazyTokenStream {
    //! token buffer.
    std::vector<Token> m_tokens;

    //! index of the current token.
    int cur = -1;

    //! State of the traversal.
    //! Used in [parser.hpp].
    TState state = TState::NotYet;

    LazyTokenStream() : m_tokens() {}

    void reset() {
        cur = -1;
    }

    inline int self_size() const {
        return static_cast<int>(m_tokens.size());
    }

    inline bool reached_eof(int k) const {
        // means we want to check the first token. 
        if (k == -1) {
            k++;
        }
        return (
            !m_tokens.empty() &&
            m_tokens.at(k).match(TokenKind::Eof)
        );
    }

    // Checks if the stream reached its end.
    inline bool has_next() const {
        return !reached_eof(cur);
    }

    // Pushes `token` into `m_tokens`.
    void feed(const Token& token) {
        cur++;
        m_tokens.push_back(token);
    }

    // Consumes a token from the stream.
    Option<Token> eat_one_token() {
        if(!has_next()) {
            return std::nullopt;
        } else {
            if(cur == -1) cur++;
            return std::make_optional<Token>(m_tokens.at(cur++));
        }
    }
};

#endif // TOKEN_HPP_