#ifndef WARG_PARSER_H_
#define WARG_PARSER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <filesystem>

class WargParser 
{
private:
    /// @brief flags of the parser from the command line.
    ///        help_    - user requested for help
    ///        verbose_ - mode=verbose
    ///        optimize_- user wants to optimize the compilation
    ///        debug_   - user wants to see debug logs.
    ///
    ///        BUILD_ONLY    - user chose --build
    ///        BUILD_AND_RUN - user chose --run  
    bool display_help_on_;
    bool display_cod_on_;
    bool verbose_;
    bool optimize_;
    bool debug_;

    bool BUILD_ONLY = false;
    bool BUILD_AND_RUN = false;

    std::optional<std::string> source_file_;
    std::optional<std::string> output_dir_;

public:
    auto GetSourceFile() -> std::optional<std::filesystem::path>;
    auto GetOutputDir() -> std::filesystem::path;
    auto DisplayHelp() -> void;
    auto DisplayOutputDir() -> void;
    auto Parse(int argc, char** argv) -> void;

    inline bool IsBuildOnly() { return BUILD_ONLY; }
    inline bool IsBuildAndRun() { return BUILD_AND_RUN; }
    inline bool IsDisplayHelpOn() { return display_help_on_; }
    inline bool IsDisplayOutputDirOn() { return display_cod_on_; }
    inline bool IsVerbose() { return verbose_; }
    inline bool IsOptimize() { return optimize_; }
    inline bool IsDebug() { return debug_; }
};

#endif // WARG_PARSER_H_