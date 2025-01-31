#ifndef _LEXER_H
#define _LEXER_H

#include "lexer.h"

#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <filesystem>

typedef enum KeywordKind {
	/// A function identifier.
	Func,
	/// The end of a function.
	Endf,
	/// Variable declaration
	Let,
	/// Logical if.
	If,
	/// Logical else.
	Else,
	/// Logical while.
	While,
	/// logical for.
	For,
	/// In operator. e.g ` 1 in [1, 2, 3] `.
	In,

	/// Return statement. { value }; 
	/// Note: ` return; ` will return null-value by default.
	Return,

	/// Break statement. Only within a loop.
	Break,
	/// Continue statement. Only within a loop.
	Continue,

	/// Match block, will contain [LiterKind] cases and BLOCKS as scopes.
	// See [Match-Blocks] in the docs.
	Match,

	/// @NOT_IMPLEMENTED, Struct type definition. 
	Struct,
	/// @NOT_IMPLEMENTED, Enum type definition.
	Enum,
	/// @NOT_IMPLEMENTED, Import and Import*.
	Import,
	/// Logical AND. e.g ` BOOLEAN and BOOLEAN`.
	And,
	/// Logical OR. e.g	` BOOLEAN or BOOLEAN `.
	Or,
	/// Logical NOT. e.g ` not BOOLEAN `.
	Not,

	/// @NOT_IMPLEMENTED, faster math operator
	///  Instead of ++var and --var, wombat uses INC and DEC. 
	///
	///  INC, increment the value by 1. e.g ` inc var; `
	Inc,
	/// DEC, decrement the value by 1. e.g ` dec var; `
	Dec,

	/// Additive operator.
	Add,
	/// Subtraction operator.
	Sub,
	/// Multiplication operator.
	Mul,
	/// Division operator.
	Div,
	/// Modulus operator.
	Mod,
	/// Power operator. Symbolized by ` ** `.
	Power
};

class Source
{
public:
	/**
	 * @brief init for ` Source `. validates the given path by the user.
	 * @param path - the path to the source file.
	 */
	explicit Source(const std::filesystem::path& p) 
		: spath_(p)
	{
		this->ValidateExtension();
		this->ValidatePath();
		this->InitStream();
	};

	/**
	 * @brief called when the source file is no longer needed.
	 */
	~Source() {
		stream_.close();
		// [TODO] -> log into a [FINAL_PROCSES_LOG_FILE] the closing of the stream.
	};

	/**
	 * @brief check if the given path exists.
	 * throws a std::errc::no_such_file_or_directory.
	 * See [RESOURCE_ERROR] for more info.
	 */	
	void ValidatePath();

	/**
	 * @brief check if the given path has the correct extension.
	 * throws an std::errc::invalid_argument.
	 * See [RESOURCE_ERROR] for more info.
	 */
	void ValidateExtension();

	/**
	 * @brief opens a FILE handler to the given source file.
	 * throws an std::errc::io_error if failed.
	 */
	void InitStream();

	/**
	 * @brief Peeks the current datagram from the source without reading it. 
	 */
	int Peak();

	/**
	 * @brief reads data from the filebuf
	 */
	void ReadFromFileBuf(int offset);

	/**
	 * @brief Getter for the underlying stream.
	 */
	inline std::ifstream& GetSourceStream() { return stream_; }

private:
	std::filesystem::path spath_;
	std::ifstream stream_;
};

class Cursor 
{
public:
	/**
	 * @brief init for ` Cursor `. validates the given path by the user.
	 */
	explicit Cursor(Source& source)
		: source_(source), offset_(0), line_(0), column_(0) {};

	/**
	 * @brief destructor for ` Cursor `.
	 */
	~Cursor() = default;

	/**
	 * @brief The InnerSource getter, retrieves the _source 
	 * @return 
	 */
	Source& InnerSource() { return source_; }

	/**
	 * @brief Checks if the cursor reached the end of the file.
	 * @return *TRUE* if the cursor did not reach EOF.
	 */
	bool NotEof();

	/**
	 * @brief Reads the current character.
	 */
	const int Consume();

private:
	Source& source_; 
	int offset_;
	int line_;
	int column_;
};


class Lexer 
{
public:
	/**
	 * @brief creates a lexer for the given source.
	 * @param source - target for the lexer, loaded BEFORE the process.
	 */
	explicit Lexer(Source& source) : cursor_(source) {};

	/**
	 * @brief destructor for ` Lexer `.
	 */
	~Lexer() = default;

	void iterate();

	void lex_line();

	void lex_buffer();

private:
	Cursor cursor_;
};

#endif // _LEXER_H