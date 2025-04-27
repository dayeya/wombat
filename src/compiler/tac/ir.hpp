#ifndef IR_HPP_
#define IR_HPP_

#include "ast.hpp"

struct TacInstruction {};

struct TacInstructionGenerator {
    TacInstructionGenerator() = default;
    ~TacInstructionGenerator() = default;

    TacInstruction gen_tac_fn() {
        return TacInstruction();
    }

    TacInstruction gen_tac_fn_start() {
        return TacInstruction();
    }

    TacInstruction gen_tac_fn_end() {
        return TacInstruction();
    }

    TacInstruction gen_tac_fn_call() {
        return TacInstruction();
    }

    TacInstruction gen_tac_fn_return() {
        return TacInstruction();
    }

    TacInstruction gen_tac_var_declaration() {
        return TacInstruction();
    }

    TacInstruction gen_tac_var_assignment() {
        return TacInstruction();
    }
};

struct Ir {
    using TacGen = TacInstructionGenerator;

    TacGen tac_gen;
    std::vector<TacInstruction> instructions;

    Ir() = default;
    ~Ir() = default;
};

#endif // IR_HPP_
