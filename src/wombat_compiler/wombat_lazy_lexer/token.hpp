#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

/**
 * @enum TokenKind.
 * @brief Contains all kinds of possible and allowed tokens within the lexial bounderies of the language.
 */
enum class TokenKind {
    //!
    //! Literal.
    Literal,

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
    //! Wombat proposes readables.
    //! 
    //! let x: int = 8; 
    //! let p_x: ref<int> = address! { x };
    //!
    Readable,

    //! 
    //! Punctuators
    //!
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
    Bang,               // Symbol for '!'

    Whitespace,         // Sequence of non-meaningful characters like spaces or tabs

    ReturnSymbol,       // Return type indicator for functions, '->'
    Plus,               // Arithmetic addition symbol,          '+'
    Minus,              // Arithmetic subtraction symbol,       '-'
    Lt,                 // Less than symbol,                    '<'
    Gt,                 // Greater than symbol,                 '>'
    DoubleEq,           // Equality operator,                   '=='
    Eq,                 // Assignment operator,                 '='
    Le,                 // Less than or equal to operator,      '<='
    Ge,                 // Greater than or equal to operator,   '>='
    Eof,                // End of file marker

    LineComment,        // Single-line comment, '//'
    Foreign,            // Unknown or invalid token for the language
    None                // Indicator for an empty token
};

/**
 * @struct Token
 * @brief Represents a token identified by the lexer.
 */
struct Token {
    TokenKind kind;
    std::string value;

    /**
     * @brief Flushes the buffer of 'value' and sets the kind to 'TokenKind::None'.
     */
    void clean() {
        value.clear();
        kind = TokenKind::None;
    }

    /**
     * @brief Extends the 'value' field by one character by appending it to the end.
     * @param ch The character to append to the token's value.
     */
    void extend(char ch) {
        value += ch;
    }

    /**
     * @brief Compares the token's kind with the provided kind.
     * @param kind The kind to compare with the token's kind.
     * @return True if the token's kind matches the provided kind, false otherwise.
     */
    bool compare_kind(const TokenKind& cmp_kind) {
        return kind == cmp_kind;
    }
};

/**
 * @class TokenStream
 * @brief A vector representing the tokenization of the entire source file.
 */
class TokenStream {
public:
    /// Default constructor
    TokenStream() = default;

    /**
     * @brief appends a new token to the vector.
     * @return True if operation of successful.
     */
    auto push_token(const Token& wombat_token) -> bool;

    /**
     * @brief returns the current number of tokens tokenized by the lexer.
     */
    auto num_of_tokens() -> std::size_t;

    auto tokens() -> std::vector<Token> {
        return m_tokens; 
    }

private:
    std::vector<Token> m_tokens;
};

#endif // TOKEN_HPP_