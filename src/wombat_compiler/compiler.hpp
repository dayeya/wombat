#ifndef COMPILER_HPP_
#define COMPILER_HPP_

#include <iostream>
#include <optional>
#include <filesystem>
#include <string>

class Compiler {
public:
	Compiler() = default;
	auto start_lexing(std::string path) -> void;
};

#endif // COMPILER_HPP_

// compiler -> init_front() -> TR.lex() -> TR
// compiler -> init_front() -> new Token_Repsentation { stream, token_counter, source }