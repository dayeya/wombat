#include <iostream>
#include <iomanip>
#include <filesystem>

#include "emitter/emitter.h"
#include "warg_parser.h"

namespace fs = std::filesystem;

std::optional<fs::path> WargParser::GetSourceFile() {
    return source_file_ ? std::make_optional(fs::path(source_file_.value())) : std::nullopt;
}

fs::path WargParser::GetOutputDir() {
    return fs::path(output_dir_.value_or(fs::current_path()));
}

auto WargParser::Parse(int argc, char** argv) -> void {
    for(int c = 1; c < argc; ++c) {
        std::string carg = *(argv + c);
        if(carg == "-h" || carg == "--help") display_help_on_ = true;
        else if(carg == "-v" || carg == "--verbose") verbose_ = true;
        else if(carg == "-o" || carg == "--optimize") optimize_ = true;
        else if(carg == "-d" || carg == "--debug") debug_ = true;
        else if(carg == "-b" || carg == "--build") {
            if(!(c + 1 < argc)) {
                EMITTER(EmitPhase(Phase::PRECOMP, "build option requires a file path")); 
            }
            source_file_ = *(argv + ++c);
            BUILD_ONLY = true;
        }
        else if(carg == "-r" || carg == "--run") {
            if(!(c + 1 < argc)) {
                EMITTER(EmitPhase(Phase::PRECOMP, "perform option requires a file path"));
            }
            source_file_ = *(argv + ++c);
            BUILD_AND_RUN = true;
        }
        else if(carg == "-u" || carg == "--cod") {
            display_cod_on_ = true;
            if(c + 1 < argc) output_dir_ = *(argv + ++c);
        }
        else {
            EMITTER(EmitPhase(Phase::PRECOMP, "unknown argument, try wombat --help for more information"));
        }
    }
}

auto WargParser::DisplayHelp() -> void {
    std::cout << "Usage: wombat [OPTION] [FLAGS] [INPUT]" << "\n";
    std::cout << "Options: " << "\n";

    const int tabs = 4;
    const int flag_width = 25;
    const int description_width = 50;

    std::cout << std::setw(4) << "" << std::left << std::setw(flag_width) << "-h, --help" << std::setw(description_width) << "Display this help message" << "\n";
    std::cout << std::setw(4) << "" << std::left << std::setw(flag_width) << "-u, --cod [DIR]" << std::setw(description_width) << "Display the current output dir, changed to [DIR] if given." << "\n";
    std::cout << std::setw(4) << "" << std::left << std::setw(flag_width) << "-b, --build [FILE]" << std::setw(description_width) << "Build the source code, -o for optimization" << "\n";
    std::cout << std::setw(4) << "" << std::left << std::setw(flag_width) << "-r, --run [FILE]" << std::setw(description_width) << "Build and run executable, -o for optimization" << "\n";
    std::cout << "\n";
    std::cout << "Flags: " << "\n";
    std::cout << std::setw(4) << "" << std::left << std::setw(flag_width) << "-o, --optimize" << std::setw(description_width) << "Optimize the final output" << "\n";
    std::cout << std::setw(4) << "" << std::left << std::setw(flag_width) << "-v, --verbose" << std::setw(description_width) << "Provide additional details" << "\n";
    std::cout << std::setw(4) << "" << std::left << std::setw(flag_width) << "-d, --debug" << std::setw(description_width) << "Generate debug info" << "\n";
}

auto WargParser::DisplayOutputDir() -> void {
    std::cout << "WOMBAT::COD IS " << GetOutputDir() << "\n";
}