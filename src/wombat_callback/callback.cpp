#include <tuple>
#include "callback.hpp"

auto Callback::invoke() const -> void {
    /// call 'callback_(diag_)`
    std::apply(callback_, std::make_tuple(diag_));
}