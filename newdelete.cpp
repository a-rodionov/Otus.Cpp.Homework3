#include <cstdlib>
#include <stdio.h>
#include <new>

namespace homework3 {

  std::size_t alloc_counter = 0;

  void* malloc(std::size_t size)  throw (std::bad_alloc)
  {
    void* p = std::malloc(size);
    ++alloc_counter;
    return p;
  }

  void free(void* p) noexcept
  {
    --alloc_counter;
    std::free(p);
    return;
  }
}
