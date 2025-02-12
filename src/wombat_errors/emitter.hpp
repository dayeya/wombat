#ifndef EMITTER_H_
#define EMITTER_H_

#include <iostream> 
#include <memory>
#include <concepts>

#include "diagnostic.hpp"

class Emitter {
public:
    Emitter() = default;
    auto emit_diagnostic(const Diagnostic& diag) const -> void;
};

#endif // EMITTER_H_