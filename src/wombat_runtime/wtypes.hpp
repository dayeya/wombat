#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <functional>
#include <memory>
#include <optional>

template<typename Ret, typename ...Args>
using Closure = std::function<Ret(Args...)>;

template<typename T>
using SmartPtr = std::unique_ptr<T>;

template<typename T>
using Option = std::optional<T>;

#endif // TYPES_HPP_