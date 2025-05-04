#ifndef TYPING_HPP_
#define TYPING_HPP_

#include <stddef.h>
#include <string>
#include <format>
#include "alias.hpp"

enum class Mutability : int {
    // Immutable items. 
    // E.g 'fn int foo(x: int)'
    //                 ^^^^^^ <-- immutable.
    // Or: 'let xs: []int;'
    Immutable,
    // Mutable items.
    // E.g 'mut z: bool = true;'
    //      ^^^ <-- mutable.
    Mutable 
};

enum class Primitive : int {
    // A free pointer. 
    // 
    // E.g 'fn free main() ... end'
    Free,
    // An integer.
    Int, 
    // A float.
    Float,
    // One byte char.
    Char,
    // A boolean (true | false)
    Boolean
};

std::string mut_str(const Mutability& mut);
Option<Primitive> maybe_primitive(const std::string& str);

enum class TypeFamily : int {
    // A primitive type.
    // E.g 'int', 'float', 'char', 'bool'
    Primitive,
    // A pointer type.
    Pointer,
    // An array type.
    Array
};

// Each type will be hashed for type checking.
struct TypeHash {
    size_t hash;

    TypeHash() : hash{0} {}
    TypeHash(size_t h) : hash{h} {}

    bool operator==(const TypeHash& other) const {
        return hash == other.hash;
    }

    std::string as_str() {
        // Hexa
        return std::format("0x{:x}", hash);
    }
};

struct Type {
    TypeFamily fam;

    virtual ~Type() = default;
    Type(TypeFamily family) : fam(family) {}

    virtual std::string as_str() const = 0;
    virtual TypeHash hash() const = 0;
};

struct PrimitiveType : virtual public Type {
    Primitive type;

    ~PrimitiveType() override = default;
    PrimitiveType(Primitive&& type)
        : Type(TypeFamily::Primitive), type{std::move(type)} {}

    inline bool cmp(Primitive&& prim) {
        return type == prim;
    }

    std::string as_str() const override {
        switch(type) {
            case Primitive::Int: return "int";
            case Primitive::Float: return "float";
            case Primitive::Char: return "char";
            case Primitive::Boolean: return "bool";
            case Primitive::Free: return "free";
            default: {
                return "primitive_UNKNOWN";
            }
        }
    }

    TypeHash hash() const override {
        size_t h = 17;
        h = h * 31 + static_cast<size_t>(fam);
        h = h * 31 + static_cast<size_t>(type);
        return TypeHash{h};
    }
};

struct PointerType : virtual public Type {
    SharedPtr<Type> underlying;

    ~PointerType() override = default;
    PointerType(SharedPtr<Type>&& type)
        : Type(TypeFamily::Pointer), underlying(std::move(type)) {}

    std::string as_str() const override {
        return std::format("ptr<{}>", underlying->as_str());
    }

    TypeHash hash() const override {
        size_t h = 17;
        h = h * 31 + static_cast<size_t>(fam);
        h = h * 31 + underlying->hash().hash;
        return TypeHash{h};
    }
};

struct ArrayType : virtual public Type {
    size_t array_size;
    SharedPtr<Type> underlying;

    ~ArrayType() override = default;
    ArrayType(size_t&& size, SharedPtr<Type>&& type)
        : Type(TypeFamily::Array), array_size(std::move(size)), underlying(std::move(type)) {}
    
    std::string as_str() const override {
        return std::format("[{}]{}", array_size, underlying->as_str());
    }

    TypeHash hash() const override {
        size_t h = 17;
        h = h * 31 + static_cast<size_t>(fam);
        h = h * 31 + std::hash<size_t>{}(array_size);
        h = h * 31 + underlying->hash().hash;
        return TypeHash{h};
    }
};

#endif // TYPING_HPP_