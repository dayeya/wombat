#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <algorithm>
#include <regex>

#include "sym.hpp"

SharedPtr<Type> parse_type(const std::string& raw_type) {
    std::string trimmed = raw_type;
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());

    if (trimmed.starts_with("ptr<") && trimmed.ends_with('>')) {
        std::string inner = trimmed.substr(4, trimmed.size() - 5);
        return std::make_shared<PointerType>(PointerType{parse_type(inner)});
    }

    if (trimmed == "int") return std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Int});
    if (trimmed == "char") return std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Char});
    if (trimmed == "bool") return std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Boolean});
    if (trimmed == "free") return std::make_shared<PrimitiveType>(PrimitiveType{Primitive::Free});
    return nullptr;
}

Option<SymFunction> sig_to_sym(const std::string& wombat_sig) {
    std::string input = wombat_sig;
    input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());

    std::regex pattern(R"(fn\s+([a-zA-Z0-9_<>\s]+)\s+([a-zA-Z_][a-zA-Z0-9_]*)\((.*)\);)");
    std::smatch match;
    if (!std::regex_match(input, match, pattern)) {
        return std::nullopt;
    }

    std::string return_type_str = match[1].str();
    std::string function_name = match[2].str();
    std::string param_list_str = match[3].str();

    SharedPtr<Type> return_type = parse_type(return_type_str);
    if (!return_type) {
        return std::nullopt;
    }

    std::vector<Declaration::Parameter> params;
    std::stringstream ss(param_list_str);
    std::string token;
    while (std::getline(ss, token, ',')) {
        std::string param = token;
        std::string trimmed = std::regex_replace(param, std::regex("^ +| +$|( ) +"), "$1");
        if (trimmed.empty()) continue;

        std::regex param_regex(R"((mut\s+)?([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*([a-zA-Z0-9_<>\s]+))");
        std::smatch param_match;
        if (!std::regex_match(trimmed, param_match, param_regex)) {
            return std::nullopt;
        }

        bool is_mut = param_match[1].matched;
        std::string name = param_match[2].str();
        std::string type_str = param_match[3].str();

        params.emplace_back(Declaration::Parameter(
            is_mut ? Mutability::Mutable : Mutability::Immutable,
            Identifier{name},
            parse_type(type_str)
        ));
    }

    return SymFunction{std::move(params), std::move(return_type)};
}