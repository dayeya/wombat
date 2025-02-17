#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include <iostream>
#include <queue>

#include "io_file.hpp"
#include "args.hpp"
#include "emitter.hpp"
#include "callback.hpp"
#include "compiler.hpp"

//! Type-aliasing for cleaner code.
using CallbackQueue = std::queue<Callback>;
using Input = std::tuple<>;

class WInterface {
public:
    WInterface(
        Args parsed_arguments,
        Emitter diagnostic_emitter,
        std::queue<Callback> callback_stack,
        InputFile in_file,
        OutputFile out_file
    ) : args_(parsed_arguments), 
        diagnostic_emitter_(diagnostic_emitter), 
        callback_queue_(callback_stack),
        in_file(InputFile()),
        out_file(OutputFile()) {}
    
    static auto build_interface(int argc, char** argv) -> WInterface;
    
    /**
     * @brief Function which executes the given prompt.
     * See the private section for a deeper dive.  
     */
    auto execute() -> void;
    auto flush_callbacks() -> void;

private:
    Args args_;
    Emitter diagnostic_emitter_;
    std::queue<Callback> callback_queue_;
    InputFile in_file;
    OutputFile out_file;

    auto validate_input() -> void;
    auto help_information() -> void;
};

#endif // INTERFACE_HPP_