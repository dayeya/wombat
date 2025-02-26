#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <vector>
#include <memory>

using std::unique_ptr, std::make_unique;
using std::shared_ptr, std::make_shared;

/**
 * @enum TokenKind.
 * @brief Contains all kinds of possible and allowed tokens within the lexial bounderies of the language.
 */
enum class TokenKind {
    //!
    //! Literals
    LiteralInt,
    LiteralFloat,
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
    Comma,              // Symbol for ';'
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
      : value(""), 
        kind(TokenKind::None), 
        pos(std::make_pair(-1, -1)) {};

    void token_to_str() const;

    /**
     * @brief Flushes the buffer of 'value' and sets the kind to 'TokenKind::None'.
     */
    void clean() {
        value = std::string();
        kind = TokenKind::None;
        pos = std::make_pair(-1, -1);
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

struct LazyTokenStream {
    unique_ptr<Token> m_current_token;
    std::vector<unique_ptr<Token>> m_tokens;

    LazyTokenStream()
        : m_current_token(std::make_unique<Token>()), m_tokens() {}; 

    inline bool reached_end_of_stream() { 
        auto& last_token = m_tokens.back();
        return last_token->compare_kind(TokenKind::Eof); 
    }
    
    void advance_with_token(unique_ptr<Token> token);
};

#endif // TOKEN_HPP_