#pragma once

#include <cstddef>
#include <algorithm>
#include <list>
#include <bitset>
#include <stdexcept>

template<typename T,
        std::size_t ALLOC_AT_ONCE_COUNT,
        decltype(&std::malloc) custom_malloc = &std::malloc,
        decltype(&std::free) custom_free = &std::free>
class custom_allocator {

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
    : allocated_blocks{other.allocated_blocks} {}

  custom_allocator& operator=(custom_allocator&) = default;

  pointer allocate(std::size_t n ) {
    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    if(1 != n)
      throw std::invalid_argument("custom_allocator can allocate only 1 element by call");

    std::size_t position{};
    auto not_full_block = std::find_if (allocated_blocks.begin(),
                                        allocated_blocks.end(),
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

    if(allocated_blocks.end() == not_full_block) {
      auto p = custom_malloc( ALLOC_AT_ONCE_COUNT * sizeof(T) );
      if(!p)
        throw std::bad_alloc();
      allocated_blocks.push_back(std::make_pair(reinterpret_cast<pointer>(p), std::bitset<ALLOC_AT_ONCE_COUNT>{1}));
      return allocated_blocks.back().first;
    }

    not_full_block->second[position] = 1;
    return not_full_block->first + position;
  }

  void deallocate(pointer p, std::size_t n) {
    //std::cout << __PRETTY_FUNCTION__ << std::endl;

    auto allocated_block = std::find_if (allocated_blocks.begin(),
                                         allocated_blocks.end(),
                                         [p] (const auto& block_description)
                                         {
                                          return ((p >= block_description.first)
                                                  && (p <= block_description.first + block_description.second.size() - 1));
                                         });
    if(allocated_blocks.end() == allocated_block)
      return;
    allocated_block->second[p - allocated_block->first] = 0;
    if(allocated_block->second.none()) {
      custom_free(allocated_block->first);
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

  std::list<std::pair<pointer, std::bitset<ALLOC_AT_ONCE_COUNT>>> allocated_blocks;
};
