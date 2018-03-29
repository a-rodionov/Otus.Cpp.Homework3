#pragma once

#include <cstddef>

constexpr std::size_t factorial(const std::size_t number) noexcept {
  if(1 >= number)
    return 1;
  return number * factorial(number-1);
};

template<std::size_t N>
struct factorial_t {
  static const std::size_t value = N * factorial_t<N-1>::value;
};

template<>
struct factorial_t<0> {
  static const std::size_t value = 1;
};
