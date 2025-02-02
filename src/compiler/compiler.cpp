#include <iostream>
#include <filesystem>

#include "compiler.h"

using namespace std;
namespace fs = std::filesystem;

/// [NOTE], could be an error.
auto Compiler::Exec() -> void {
	if (warg_parser_->IsBuildOnly()) Build();
}

auto Compiler::Build() -> void {
	std::cout << "Hello from Compiler::Build()" << std::endl;
}