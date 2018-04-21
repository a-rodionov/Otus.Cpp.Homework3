#pragma once

#include <cstddef>
#include <type_traits>

namespace homework3 {

constexpr auto factorial(auto number) -> decltype(number)
{
  static_assert(std::is_integral<decltype(number)>::value, "Argument of factorial function must be integer type.");
  if(0 > number)
    throw std::invalid_argument("Argument of factorial function must be positive integer.");

  return number ? number * factorial(number-1) : 1;
}

template<std::size_t N>
struct factorial_t
{
  static const std::size_t value = N * factorial_t<N-1>::value;
};

template<>
struct factorial_t<0>
{
  static const std::size_t value = 1;
};

}
