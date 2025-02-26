#ifndef CALLBACK_HPP_
#define CALLBACK_HPP_

#include <iostream>
#include <optional>
#include <tuple>
#include <functional>

#include "diagnostic.hpp"

enum class CallbackId {
    DiagnosticRendering,
};

/**
 * @brief Callback class represents the one of the main concepts of wombats UX and error management.
 * A callback is a function that will execute whenever the main 'interface' / 'driver' of wombat-compiler performs and early exit.
 * Any callback will be visualized by a cmd-block of information regarding the information about the error origin.  
 */
class Callback {
public:
    /// Ensure that *any* callback must return *void* and emit a diagnostic. 
    using Fn = std::function<const void(const Diagnostic&)>;

    Callback(CallbackId cb_iden, Fn func, const Diagnostic& diagnostic)
        : id_(cb_iden), 
          callback_(std::move(func)),
          diag_(diagnostic) {}

    /**
     * @brief Invokes the inner callback_ function from the callback-stack.
     * If given callback fails than 
     * @return std::optional<RetType>
     */
    auto invoke() const -> void;

private:
    CallbackId id_;
    Fn callback_;
    Diagnostic diag_;
};

#endif // CALLBACK_HPP_