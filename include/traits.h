#ifndef INCLUDE_TRAITS_H
#define INCLUDE_TRAITS_H

#include <concepts>

namespace data {
    template <typename T>
    concept PartialOrd = requires(T a) {
        { a < a } -> std::convertible_to<bool>;
    };

    template <typename T>
    concept Eq = requires(T a) {
        { a == a } -> std::convertible_to<bool>;
    };
}

#endif
