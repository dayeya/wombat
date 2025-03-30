#include "den.hpp"
#include "common.hpp"

int main_windows(int argc, char** argv) {
    TODO(
        "IMPLEMENT WINDOWS COMPATIBILITY",
        "WOMBAT v0.1.0",
        "wombat::main_windows"
    );
    return 0;
}

int main_linux(int argc, char** argv) {
    Den den = Den();
    den.init(argv[0]);
    den.parse(argc, argv);
    return 0;
}

int main(int argc, char** argv) {
    main_linux(argc, argv);
    return 0;
}