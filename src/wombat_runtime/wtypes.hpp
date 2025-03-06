#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <functional>
#include <memory>

template<typename Ret, typename ...Args>
using Closure = std::function<Ret(Args...)>;

template<typename T>
using SmartPtr = std::unique_ptr<T>;

#endif // TYPES_HPP_