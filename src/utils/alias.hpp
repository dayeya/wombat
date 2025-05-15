#ifndef ALIAS_HPP_
#define ALIAS_HPP_

#include <functional>
#include <memory>
#include <optional>
#include <expected>
#include <variant>

template<typename Ret, typename ...Args>
using Closure = std::function<Ret(Args...)>;

template<typename T>
using Ptr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using RawPtr = T*;

template<typename T>
using Option = std::optional<T>;

template<typename T, typename E>
using Result = std::expected<T, E>;

template<typename E>
using ErrResult = Result<std::monostate, E>;

template<typename T> 
Ptr<T> mk_ptr(T&& v) {
    return std::make_unique<T>(std::forward<T>(v));
}

// Wrapper around std::string when we use file locations.
using StrLoc = std::string;

// Wrapper around std::string when we cheap identifier without including [token.hpp].
using CheapIdent = std::string;

#endif // ALIAS_HPP_