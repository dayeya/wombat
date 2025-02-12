#ifndef CALLBACK_HPP_
#define CALLBACK_HPP_

#include <iostream>
#include <optional>
#include <functional>
#include <tuple>

/**
 * @brief Callback class represents the one of the main concepts of wombats UX and error management.
 * A callback is a function that will execute whenever the main 'interface' / 'driver' of wombat-compiler performs and early exit.
 * Any callback will be visualized by a cmd-block of information regarding the information about the error origin.  
 */
template<typename RetType, typename ...Args>
class Callback {
public:
    /// Ensuring all derived callbacks are <std::function>
    using Fn = std::function<RetType(Args...)>;

    Callback(int callbackId, Fn func, Args... args) 
        : id_(callbackId), 
          callback_(std::move(func)), 
          args_(
            std::make_tuple(std::forward<Args>(args)...)  
        ) {}

    /**
     * @brief Invokes the inner callback_ function from the callback-stack.
     * If given callback fails than 
     * @return std::optional<RetType>
     */
    auto invoke() -> std::optional<RetType>;

protected:
    auto try_callback() -> RetType;

private:
    int id_;
    Fn callback_;
    std::tuple<Args...> args_;
};

#endif // CALLBACK_HPP_