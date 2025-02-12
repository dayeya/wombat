#include <iostream>
#include "interface.hpp"

/// [TODO] Load ` gugup.mp3 ` whenever the user uses wombat, allow to disable of course.

int main(int argc, char** argv) {
    auto wombat_interface = WInterface::fromInput(argc, argv);
    return 0;
}
