#pragma once

#include <cstddef>
#include <algorithm>
#include <list>
#include <bitset>
#include <stdexcept>
#include <memory>

template<typename T,
        std::size_t ALLOC_AT_ONCE_COUNT,
        decltype(&std::malloc) custom_malloc = &std::malloc,
        decltype(&std::free) custom_free = &std::free>
class custom_allocator {

  static_assert(0 != ALLOC_AT_ONCE_COUNT, "2nd template parameter must be not equal to 0.");

public:

  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template<typename U> struct rebind { typedef custom_allocator<U, ALLOC_AT_ONCE_COUNT, custom_malloc, custom_free> other; };

  custom_allocator() = default;

  custom_allocator(custom_allocator&& other)
    : allocated_blocks{std::forward<decltype(allocated_blocks)>(other.allocated_blocks)} {}

  custom_allocator& operator=(const custom_allocator&) = default;

  pointer allocate(std::size_t n ) {
    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    if(1 != n)
      throw std::invalid_argument("custom_allocator can allocate only 1 element by call");

    std::size_t position{};
    auto not_full_block = std::find_if (std::begin(allocated_blocks),
                                        std::end(allocated_blocks),
                                        [&position] (const auto& block_description)
                                        {
                                          if(block_description.second.all())
                                            return false;
                                          
                                          for(position = 0; position < block_description.second.size(); ++position) {
                                            if(!block_description.second[position])
                                              return true;
                                          }
                                          return false;
                                        });

    if(std::end(allocated_blocks) == not_full_block) {
      auto p = custom_malloc( ALLOC_AT_ONCE_COUNT * sizeof(T) );
      if(!p)
        throw std::bad_alloc();
      allocated_blocks.push_back(std::make_pair(std::unique_ptr<void, decltype(&std::free)>(p, custom_free),
                                                std::bitset<ALLOC_AT_ONCE_COUNT>{1}));
      return reinterpret_cast<pointer>(allocated_blocks.back().first.get());
    }

    not_full_block->second[position] = 1;
    return reinterpret_cast<pointer>(not_full_block->first.get()) + position;
  }

  void deallocate(pointer p, std::size_t n) {
    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    auto allocated_block = std::find_if (std::begin(allocated_blocks),
                                         std::end(allocated_blocks),
                                         [p] (const auto& block_description)
                                         {
                                          return ((p >= reinterpret_cast<pointer>(block_description.first.get()))
                                                  && (p <= reinterpret_cast<pointer>(block_description.first.get()) + block_description.second.size() - 1));
                                         });
    if(std::end(allocated_blocks) == allocated_block)
      return;
    allocated_block->second[p - reinterpret_cast<pointer>(allocated_block->first.get())] = 0;
    if(allocated_block->second.none()) {
      allocated_blocks.erase(allocated_block);
    }
  }

  template<typename ... Args >
  void construct(pointer p, Args&&... args) {
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    new(p) T{std::forward<Args>(args)...};
  }

  void destroy(pointer p) {
    //std::cout << __PRETTY_FUNCTION__ << std::endl;
    p->~T();
  }

private:

  std::list<std::pair<std::unique_ptr<void, decltype(&std::free)>, std::bitset<ALLOC_AT_ONCE_COUNT>>> allocated_blocks;
};
