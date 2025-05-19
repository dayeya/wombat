#ifndef ASM_HPP_
#define ASM_HPP_

#include <stack>
#include "ir.hpp"

enum class Register: int {
    Rax,
    Rbx,
    Rdi,
    Rsi,
    Rdx,
    Rcx,
    R8,
    R9
};

enum class AllocRegion: int {
    Stack, 
    Heap
};

struct AsmVar {
    // Offest from the beginning of the region. (from rbp).
    size_t offset;
    // Size in bytes.
    size_t memsize;
    // In 'what' region does the variable live. heap / stack.
    AllocRegion where;

    AsmVar() : offset{0}, memsize{0}, where{AllocRegion::Stack} {}
    AsmVar(size_t offset, size_t&& memsize, AllocRegion&& where) 
        : offset{std::move(offset)}, 
          memsize{std::move(memsize)},
          where{std::move(where)} {}
}; 

struct AsmStackFrame {
    using AsmIdent = std::string;
    using _Table = std::unordered_map<AsmIdent, AsmVar>;

    // Debug data.
    size_t seq_num;
    String name;

    // Context.
    _Table frame;
    size_t offset = 0;
    size_t total_size = 0;
    size_t aligned_size = 0;

    AsmStackFrame(size_t seq_num, String name) 
        : seq_num{std::move(seq_num)},
          name{std::move(name)}, 
          frame() {}

    void update_offest(size_t chunk) {
        offset += chunk;
    }

    void alloc(String&& name, size_t&& size) {
        ASSERT(frame.count(name) == 0, format("[codegen::err] cannot 'realloc', used on '{}'", name));

        update_offest(size);
        frame[name] = AsmVar { offset, std::move(size), AllocRegion::Stack };

        total_size += size;
        align_size();
    }

    int var_offset(const String& name) const {
        auto it = frame.find(name);
        ASSERT(
            it != frame.end(),
            format("variable not found in stack frame: {}", name)
        );
        return it->second.offset;
    }

    size_t var_memsize(const String& name) const {
        auto it = frame.find(name);
        ASSERT(
            it != frame.end(),
            format("variable not found in stack frame: {}", name)
        );
        return it->second.memsize;
    }


    void align_size() {
        if(total_size % 16 == 1) {
            aligned_size  = (total_size / 16 + 1) * 16;
        } else {
            aligned_size = total_size;
        }
    }

    size_t allocated_bytes() const {
        return aligned_size;
    } 
    
    void clear() {
        frame.clear();
    }
};

struct AsmStack {
    using _Stack = std::stack<AsmStackFrame>;

    _Stack stack;
    size_t cur_seq_num;

    AsmStack() : stack() {}

    void enter_func(String name) {
        // generate a new frame.
        stack.emplace(AsmStackFrame(cur_seq_num++, name));
    }

    // An assert for various kinds of operations on the stack.
    void _core_assert(const String& log) const {
        ASSERT(!stack.empty(), log);
    }

    void exit_func() {
        _core_assert("cannot exit from a nonexistent frame");
        stack.pop();
    }

    void allocate(String&& name, size_t size) {
        _core_assert("no active functions.");
        stack.top().alloc(std::move(name), std::move(size));
    }

    size_t offset(const String& name) const {
        _core_assert("no active functions.");
        return stack.top().var_offset(name);
    }

    size_t memsize(const String& name) const {
        _core_assert("no active functions.");
        return stack.top().var_memsize(name);
    }

    size_t stack_size() const {
        _core_assert("no active functions.");
        return stack.top().allocated_bytes();
    }

    void align_stack() {
        _core_assert("no active functions.");
        stack.top().align_size();
    }
};

#endif // ASM_HPP_