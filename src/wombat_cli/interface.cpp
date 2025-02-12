#include <iomanip>
#include <string>
#include <vector>

#include "args.hpp"
#include "emitter.hpp"
#include "interface.hpp"

auto WInterface::fromInput(int argc, char** argv) -> WInterface {
    auto wombat_emitter = Emitter();
    auto parse_result = Args::parse_args(argc, argv);

    if(!parse_result.has_value()) {
        wombat_emitter.emit_diagnostic(parse_result.error());
    }

    return WInterface();
}

auto WInterface::displayHelp() -> void {
    std::cout << "wombat [OPTIONS] [INPUTS]";
}