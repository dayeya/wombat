#ifndef TOKEN_DEF_H_
#define TOKEN_DEF_H_

enum class TokenKind {
	/// A line comment. e.g ` # comment `
	LineComment,
	/// An identifier that is a keyword.
	/// Jump to [KeyWordKind] for more information.
	KeywordKind,
	/// An identifier that is invalid.
	NotAKeyword,
	/// Literals, raw values like: "hello", 123, 0xAF etc.
	/// Jump to [LiteralKind] for more information.
	Literal,
	/// Any stream of characters that are referencing something.
	Indentifier,
	/// Any whitespace margin.
	Whitespace,
	/// `;`
	Semi,
	/// `,`
	Comma,
	/// `.`
	Dot,
	/// `(`
	OpenParen,
	/// `)`
	CloseParen,
	/// `{`
	OpenBrace,
	/// `}`
	CloseBrace,
	/// `[`
	OpenBracket,
	/// `]`
	CloseBracket,
	/// `@`
	At,
	/// `#`
	Pound,
	/// `~`
	Tilde,
	/// `?`
	Question,
	/// `:`
	Colon,
	/// `$`
	Dollar,
	/// `=`
	Eq,
	/// `!`
	Bang,
	/// `<`
	Lt,
	/// `>`
	Gt,
	/// `-`
	Minus,
	/// `&`
	BiteiseAnd,
	/// `|`
	BitwiseOr,
	/// `+`
	Plus,
	/// `*`
	Star,
	/// `/`
	ForwardSlash,
	/// `^`
	Caret,
	/// `%`
	Percent,
	// Foreign token, not expected by the lexer.
	Foreign,
	/// End of input.
	Eof
};

enum class	LiteralKind {
	/// A string literal.
	Str,
	/// A byte string literal. b"hello", b"world" etc.
	ByteStr,
	/// An Integer literal. '0xaf', '0b1010', '123' etc.
	Int,
	/// A float literal. 8.4f
	Float,
	/// A boolean literal. true or false.
	Boolean,
	/// A character literal.
	Char,
	/// A byte literal. b"c", b"\\";
	Byte,
	/// A null literal.
	Null
};

#endif // TOKEN_DEF_H_
