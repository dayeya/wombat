#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include <vector>

#include "args.hpp"
#include "emitter.hpp"
#include "callback.hpp"
#include "compiler.hpp"

class WInterface {
public:
    WInterface(
        Args parsed_arguments,
        Emitter diagnostic_emitter,
        std::vector<Callback> callback_stack
    ) : args_(parsed_arguments), diagnostic_emitter_(diagnostic_emitter), callback_stack_(callback_stack) {}
    
    static auto build_interface(int argc, char** argv) -> WInterface;
    
    /**
     * @brief Function which executes the given prompt.
     *        See the private section for a deeper dive.  
     */
    auto execute() -> void;

private:
    Args args_;
    Emitter diagnostic_emitter_;
    std::vector<Callback> callback_stack_;

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
    static auto help_information() -> void;
};

#endif // INTERFACE_HPP_