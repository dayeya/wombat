#ifndef EMITTER_H_
#define EMITTER_H_

#include <iostream> 
#include <concepts>

template <typename T>
concept Emitable = requires(T emitable) {
    {
        emitable.emit()
    } -> std::same_as<void>;
};

enum class Phase { PRECOMP, LEXING, PARSING, CODEGEN, OPTIMIZE, VALIDATION };

constexpr auto ptos(Phase p) -> const char* {
    switch (p) {
        case Phase::LEXING:     return "LEXING";
        case Phase::PRECOMP:    return "PRECOMP";
        case Phase::PARSING:    return "PARSING";
        case Phase::CODEGEN:    return "CODEGEN";
        case Phase::OPTIMIZE:   return "OPTIMIZE";
        case Phase::VALIDATION: return "VALIDATION";
        default:
            return "WOMBAT__EMITTER::PHASE = UNKNOWN";
    }
}

struct EmitPhase {
    Phase phase_;
    std::string inner_;

    EmitPhase(Phase phase, std::string inner) 
        : phase_(phase), inner_(inner) {}

    auto emit() const -> void {
        std::cerr << "WOMBAT_EMITTER::PHASE = " << ptos(phase_) << std::endl;
        std::cerr << "\t" << inner_ << std::endl;
    }
};

template <typename Emitable>
auto EMITTER(const Emitable& emitable) -> void {
    emitable.emit();
    exit(1);
};

#endif // EMITTER_H_