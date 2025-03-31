#include "common.hpp"
#include "builder.hpp"
#include "compiler.hpp"

int main_windows(int argc, char** argv) {
    TODO(
        "Implement Windows Compatibility",
        "Den v0.1.0",
        "wombat::main_windows"
    );
    return 0;
}

int main_linux(int argc, char** argv) {
    // Builder is just a wrapper around the build configuration.
    Builder wombat_builder = Builder();
    wombat_builder.init(*argv);

    // This fills the 'config' of the builder.
    wombat_builder.parse_arguments(argc, argv + 1);

    Compiler compiler = Compiler();
    compiler.init_config(wombat_builder.config);

    compiler.compile_target();
    return 0;
}

int main(int argc, char** argv) {
    main_linux(argc, argv);
    return 0;
}