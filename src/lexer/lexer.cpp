#include <iostream>
#include <filesystem>

#include "lexer.h"
#include "emitter/emitter.h"

namespace fs = std::filesystem;

constexpr const char* WOMEXTENSION = ".wo";

auto Source::ValidatePath() -> void {
	if (!fs::exists(fs::absolute(spath_))) {
		EMITTER(EmitPhase(Phase::VALIDATION, "could not resolve given path."));	
	}
}

auto Source::ValidateExtension() -> void {
	if (!(spath_.extension() == WOMEXTENSION)) {
		EMITTER(EmitPhase(Phase::VALIDATION, "extension validation failed, .wo expected."));
	}
}

void Source::InitStream() {
	stream_.open(spath_);
	if (!stream_.is_open()) {
		EMITTER(EmitPhase(Phase::PRECOMP, "source::init_stream failed, could not open file."));
	}
}

auto Cursor::NotEof() -> bool {
	std::ifstream& ifs = source_->GetSourceStream();
	return !ifs.eof();
}

auto Cursor::Read() -> char {
	std::ifstream& ifs = source_->GetSourceStream();
	std::filebuf* pbuf = ifs.rdbuf();

	// Read from the buffer, then advance to the next.
	const int c = pbuf->sbumpc();
	return c;
}

auto Lexer::Cat() -> void {
	std::cout << "Hello! from Lexer::Cat()" << std::endl;
}