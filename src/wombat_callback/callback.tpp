#include <optional>
#include "callback.hpp"

template<typename RetType, typename ...Args>
auto Callback<RetType, Args...>::tryCallback() -> RetType {
    return std::apply(callback_, args_); // Invokes the callable with args
}

template<typename RetType, typename ...Args>
auto Callback<RetType, Args...>::invoke() -> std::optional<RetType> {
    auto cb_res = tryCallback();
    return std::make_optional<RetType>(cb_res);
}