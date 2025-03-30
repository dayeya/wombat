#ifndef ALIAS_HPP_
#define ALIAS_HPP_

#include <functional>
#include <memory>
#include <optional>
#include <expected>

template<typename Ret, typename ...Args>
using Closure = std::function<Ret(Args...)>;

template<typename T>
using Ptr = std::unique_ptr<T>;

template<typename T>
using RawPtr = T*;

template<typename T>
using Option = std::optional<T>;

template<typename T, typename E>
using Result = std::expected<T, E>;

template<typename E>
using ErrResult = std::unexpected<E>;

template<typename T> 
Ptr<T> mk_ptr(T&& v) {
    return std::make_unique<T>(std::forward<T>(v));
}

#endif // ALIAS_HPP_