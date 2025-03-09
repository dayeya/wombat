#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <vector>
#include <string>

#include "wtypes.hpp"

/**
 * @enum TokenKind
 * @brief Defines all possible tokens within the lexical boundaries of Wombat.
 */
enum class TokenKind {
    //!
    //! Literals
    LiteralNum,
    LiteralString,
    LiteralChar,
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
    //! let p_x: ref<int> = address! { x };
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

    ReturnSymbol,       // Return type indicator for functions, '->'
    Plus,               // Arithmetic addition symbol,          '+'
    Minus,              // Arithmetic subtraction symbol,       '-'
    Lt,                 // Less than symbol,                    '<'
    Gt,                 // Greater than symbol,                 '>'
    DoubleEq,           // Equality operator,                   '=='
    NotEq,           // Equality operator,                   '!='
    Eq,                 // Assignment operator,                 '='
    Le,                 // Less than or equal to operator,      '<='
    Ge,                 // Greater than or equal to operator,   '>='
    Eof,                // End of file marker

    LineComment,        // Single-line comment, '//'
    Foreign,            // Unknown or invalid token for the language
    None                // Indicator for an empty token
};

//! Token stream state.
enum class TState {
    NotYet,
    Traversing,
    Ended
};

/**
 * @brief Converts a TokenKind to its string representation.
 * @param kind The token kind.
 * @return The corresponding string.
 */
std::string kind_to_str(const TokenKind& kind);

/**
 * @struct Token
 * @brief Represents a token identified by the lexer.
 */
struct Token {
    TokenKind kind;
    std::string value;
    std::pair<int, int> pos; // [line, col] - start of the token.

    Token()
        : kind(TokenKind::None), value(""), pos{-1, -1} {}

    Token(TokenKind k, std::string v, std::pair<int, int> p)
        : kind(k), value(std::move(v)), pos(p) {}

    void out() const;

    /**
     * @brief Resets the token to an empty state.
     */
    void clean() {
        value.clear();
        kind = TokenKind::None;
        pos = {-1, -1};
    }

    /**
     * @brief Appends a character to the token's value.
     */
    void extend(char ch) {
        value += ch;
    }

    /**
     * @brief Checks if the token's kind matches the given kind.
     */
    bool compare_kind(TokenKind cmp_kind) const {
        return kind == cmp_kind;
    }

    /**
     * @brief Sets the token's properties.
     */
    void fill_with(std::string v, TokenKind k, int line = -1, int col = -1) {
        value = std::move(v);
        kind = k;
        pos = {line, col};
    }

    void set_value(std::string&& v) {
        value = std::move(v);
    }

    /**
     * @brief Sets the token's kind.
     */
    void set_kind(TokenKind k) {
        kind = k;
    }

    /**
     * @brief Sets the token's position.
     */
    void set_pos(int line, int col) {
        pos = {line, col};
    }
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

    inline int self_size() const {
        return static_cast<int>(m_tokens.size());
    }

    inline bool reached_eof() const {
        return (
            !m_tokens.empty() &&
            m_tokens.back().compare_kind(TokenKind::Eof)
        );
    }

    //! Checks if the stream reached its end.
    inline bool has_next() const {
        return !m_tokens.empty() && cur < self_size() && !reached_eof();
    }

    //! Pushes `token` into `m_tokens`.
    void feed(const Token& token) {
        m_tokens.push_back(token);
    }

    //! Consumes a token from the stream.
    Option<Token> eat_one_token() {
        if(!has_next()) {
            return std::nullopt;
        } else {
            return std::make_optional<Token>(m_tokens.at(++cur));
        }
    }
};

#endif // TOKEN_HPP_