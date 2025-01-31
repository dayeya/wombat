#include <iostream>
#include <filesystem>

#include "lexer.h"

namespace fs = std::filesystem;

void Source::ValidatePath() {
	if (fs::exists(this->source_path)) {
		// [TODO] -> log into a [FINAL_PROCSES_LOG_FILE] the test of path.
		return;
	}
}

void Source::ValidateExtension() {
	if (this->source_path.extension() == WOM_EXTENSION) {
		// [TODO] -> log into a [FINAL_PROCSES_LOG_FILE] the test of ext.
		return;
	}
	else {
		// Emit an error.
	}
}

void Source::InitStream() {
	this->stream.open(this->source_path);
	if (this->stream.is_open()) {
		// [TODO] -> log into a [FINAL_PROCSES_LOG_FILE] the test of stream.
		return;
	}
}

bool Cursor::NotEof() {
	std::ifstream ifs = this->SourceStream();
	return !ifs.eof();
}

char Cursor::Read() {
	std::ifstream ifs = this->source.source_stream();
	std::filebuf* pbuf = ifs.rdbuf();

	// Read from the buffer, then advance to the next.
	const int c = pbuf->sbumpc();
	
	

	return c;
}

void Lexer::LexSource() {
	while (this->cursor.not_eof()) {
		char c = this->cursor.next();
		std::cout << c;
	}
}
