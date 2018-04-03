#include "custom_allocator.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <map>

template<typename Container, typename Generator>
auto Benchmark(std::size_t iterations, Generator generator)
{
  Container container;

  auto start = std::chrono::high_resolution_clock::now();
  std::generate_n(std::inserter(container, std::begin(container)),
                  iterations,
                  generator);
  auto end = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
}

int main(int argc, char const *argv[])
{
  try
  {
    const std::size_t iterations{100000};
    auto pair_generator = [i=0] () mutable {
      auto value = std::make_pair(i, i);
      ++i;
      return value;
    };

    std::cout << "Benchmark of std::allocator and custom_allocator used by std::map\n";
    std::cout << "Iteration: = " << iterations << "\n";
    std::cout << "elapsed time for std::allocator: " 
              << Benchmark<std::map<int, int>>(iterations, pair_generator) << "ms\n";
    std::cout << "elapsed time for custom_allocator with block size for 1000 elements: " 
              << Benchmark<std::map<int, int, std::less<int>, custom_allocator<std::pair<const int, int>, 1000>>>(iterations, pair_generator) << "ms\n";
    std::cout << "elapsed time for custom_allocator with block size for 100 elements: " 
              << Benchmark<std::map<int, int, std::less<int>, custom_allocator<std::pair<const int, int>, 100>>>(iterations, pair_generator) << "ms\n";
    std::cout << "elapsed time for custom_allocator with block size for 10 elements: " 
              << Benchmark<std::map<int, int, std::less<int>, custom_allocator<std::pair<const int, int>, 10>>>(iterations, pair_generator) << "ms\n";
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
