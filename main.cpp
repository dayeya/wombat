#include <iostream>
#include "interface.hpp"

/// [TODO] Load ` gugup.mp3 ` whenever the user uses wombat, allow to disable of course.

int main(int argc, char** argv) {
    //! Main entry point into wombat.
    auto wombat_interface = WInterface::build_interface(argc, argv);

    //! Execute the prompted query.
    //! Any errors/diagnostics will be inserted to the callback-queue.
    wombat_interface.execute();

    //! Catch early errors.
    //! They can manifest during input validation or argument parsing.
    wombat_interface.flush_callbacks();
    return 0;
}
