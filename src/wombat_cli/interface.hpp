#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include "args.hpp"
#include "emitter.hpp"

class WInterface {
public:
    static auto fromInput(int argc, char** argv) -> WInterface;
    
    /**
     * @brief Function which executes the given prompt.
     *        See the private section for a deeper dive.  
     */
    auto execute() -> void;

private:
    /**
     * @brief Changes the output dir of the compiler, which contains all '.exe' files.
     *        Invoked when using --outdir | -u
     */
    auto setOutputDir() -> void;

    /**
     * @brief Displays the output dir *full* path. 
     *        Invoked when using  --outdir | -u [INPUT]
     */
    auto displayOutputDir() -> void;

    /** @brief Displays an 'help' message to the user, cannot be overloaded on top of other options.
     *         Invoked when using --help | -h.
     */
    static auto displayHelp() -> void;
};

#endif // INTERFACE_HPP_