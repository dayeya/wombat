#ifndef COMPILER_H 
#define _COMPILER_H

#include  "lexer/lexer.h"
#include  "warg_parser/warg_parser.h"

#include <iostream>
#include <optional>
#include <filesystem>
#include <string>

using std::unique_ptr;

class Compiler
{
private: 
	unique_ptr<WargParser> warg_parser_;
	unique_ptr<Lexer> lexer_;

public:

	/**
	 * @brief creates the compiler. defaults the cud to thr cwd.	
	 */
	Compiler(unique_ptr<WargParser> warg_parser, unique_ptr<Lexer> lexer) 
		: warg_parser_{std::move(warg_parser)}, 
		  lexer_{std::move(lexer)} {}

	/**
	 * @brief called when the compiler finishes an execution. flushes the memory.
	 */
	~Compiler() = default;

	/**
	 * @brief Executes the current setting.
	 * 		  BUILD_ONLY will only compile the source code, 
	 * 		  BUILD_AND_RUN will compile and run the executable.
	 */
	auto Exec() -> void;
	
	/// @brief Builds the source file and generates a runnable executable into the the current output dir. 
	/// @return void. In the future -> wom_result<Build>();
	auto Build() -> void;
};

#endif // _COMPILER_H
