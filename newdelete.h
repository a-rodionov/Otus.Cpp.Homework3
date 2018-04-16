#pragma once

#include <stdint.h>
#include <cstddef>
#include <new>

namespace homework3 {

    extern std::size_t alloc_counter;
    void* malloc(std::size_t size) throw (std::bad_alloc);
    void free(void* p) noexcept;

}

extern "C++" {

inline void* operator new(std::size_t size) throw (std::bad_alloc)
{
    return homework3::malloc(size);
}

inline void operator delete(void* p) noexcept
{
    homework3::free(p);
}

inline void* operator new[](std::size_t size) throw (std::bad_alloc)
{
    return homework3::malloc(size);
}

inline void operator delete[](void* p) throw()
{
    homework3::free(p);
}

inline void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
  return homework3::malloc(size);
}

inline void operator delete(void* p, const std::nothrow_t&) noexcept
{
    homework3::free(p);
}

inline void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
  return homework3::malloc(size);
}

inline void operator delete[](void* p, const std::nothrow_t&) noexcept
{
    homework3::free(p);
}

inline void operator delete(void* p, long unsigned int)
{
    homework3::free(p);
}

inline void operator delete [](void* p, long unsigned int)
{
    homework3::free(p);
}

} // extern "C++"
