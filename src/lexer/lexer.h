#ifndef _LEXER_H
#define _LEXER_H

#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <filesystem>

using std::unique_ptr;

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

	/// @brief called when the source file is no longer needed.
	~Source() {
		stream_.close();
		// [TODO] -> log into a [FINAL_PROCSES_LOG_FILE] the closing of the stream.
	};

	/// @brief Validates the path of the given source, emits an EmitPhase::VALIDATION
	/// 	   See [emitter.h] for more information.
	void ValidatePath();

	/// @brief Validates the extension of the source file, emits an EmitPhase::VALIDATION
	/// 	   See [emitter.h] for more information.
	void ValidateExtension();

	/// @brief Opens the source file, emits an EmitPhase::PRECOMP.
	/// 	   See [emitter.h] for more information.
	void InitStream();

	/// @brief Peeks the current datagram from the source without reading it. 
	int Peak();

	void ReadFromFileBuf(int offset);

	/// @brief Getter for the buffer of the source file.
	/// @return returns an ifstream.
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
	explicit Cursor(unique_ptr<Source> source)
		: source_{std::move(source)}, offset_{0}, line_{0}, column_{0} {};

	/**
	 * @brief destructor for ` Cursor `.
	 */
	~Cursor() = default;

	/**
	 * @brief Checks if the cursor reached the end of the file.
	 * @return *TRUE* if the cursor did not reach EOF.
	 */
	bool NotEof();

	/**
	 * @brief Reads the current character.
	 */
	char Read();

private:
	unique_ptr<Source> source_; 
	int offset_;
	int line_;
	int column_;
};


class Lexer 
{
private:
	unique_ptr<Cursor> cursor_;
public:
	/**
	 * @brief creates a lexer for the given source.
	 * @param source - target for the lexer, loaded BEFORE the process.
	 */
	explicit Lexer(unique_ptr<Source> source) : cursor_{std::make_unique<Cursor>(std::move(source))} {};

	/**
	 * @brief destructor for ` Lexer `.
	 */
	~Lexer() = default;

	auto Cat() -> void;
	auto LexLine() -> void;
};

#endif // _LEXER_H