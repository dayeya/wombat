#include "typing.hpp"
#include "token.hpp"

std::string mut_str(const Mutability& mut) {
    if(mut == Mutability::Mutable) return "mut";
    if(mut == Mutability::Immutable) return "let";
    ASSERT(false, "cannot define meaning of undifined mutability");
    return "mut_UNDIFINED";
}

Option<Primitive> maybe_primitive(const std::string& str) {
    if(str.compare("int") == 0) return Primitive::Int;
    if(str.compare("float") == 0) return Primitive::Float;
    if(str.compare("ch") == 0) return Primitive::Char;
    if(str.compare("bool") == 0) return Primitive::Boolean;
    if(str.compare("free") == 0) return Primitive::Free;
    return std::nullopt;
}