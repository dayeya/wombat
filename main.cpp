#include <iostream>

#include "src/compiler/compiler.h"
#include "warg_parser/warg_parser.h"

int main(int argc, char** argv) {
    std::unique_ptr<WargParser> warg_parser = std::make_unique<WargParser>();
    warg_parser->Parse(argc, argv);
    
    if(warg_parser->IsBuildAndRun() || warg_parser->IsBuildOnly()) {
        std::unique_ptr<Source> src = std::make_unique<Source>(warg_parser->GetSourceFile().value());
        std::unique_ptr<Lexer> lexer = std::make_unique<Lexer>(std::move(src));

        Compiler comp {
            std::move(warg_parser), 
            std::move(lexer)
        };
        comp.Exec();
    } else {
        if(warg_parser->IsDisplayOutputDirOn()) warg_parser->DisplayOutputDir();
        if(warg_parser->IsDisplayHelpOn()) warg_parser->DisplayHelp();
    }
}
