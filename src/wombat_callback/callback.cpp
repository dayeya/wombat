#include <tuple>
#include "callback.hpp"

auto Callback::invoke() const -> void {
    std::apply(callback_, std::make_tuple(diag_));
}