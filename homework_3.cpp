#include <map>
#include "homework_3.h"
#include "utils.h"
#include "custom_allocator.h"
#include "custom_forward_list.h"
#include "newdelete.h"

namespace homework3 {

static const int ALLOCATE_AT_ONCE_SIZE = 10;

void homework_3(std::ostream& out)
{
  auto pair_generator = [i=0] () mutable {
    auto value = std::make_pair(i, factorial(i));
    ++i;
    return value;
  };

  auto sequence_generator = [i=0] () mutable {
    return i++;
  };

  std::map<int, int> map_default_allocator;
  std::generate_n(std::inserter(map_default_allocator, std::begin(map_default_allocator)),
                  ALLOCATE_AT_ONCE_SIZE,
                  pair_generator);

  std::map<int, int, std::less<int>, custom_allocator<std::pair<const int, int>, ALLOCATE_AT_ONCE_SIZE>> map_custom_allocator;
  std::generate_n(std::inserter(map_custom_allocator, std::begin(map_custom_allocator)),
                  ALLOCATE_AT_ONCE_SIZE,
                  pair_generator);

  for(const auto& pair : map_custom_allocator)
    out << pair.first << ' ' << pair.second << std::endl;


  custom_forward_list<int> custom_forward_list_default_allocator;
  std::generate_n(std::front_inserter(custom_forward_list_default_allocator),
                  ALLOCATE_AT_ONCE_SIZE,
                  sequence_generator);

  custom_forward_list<int, custom_allocator<int, ALLOCATE_AT_ONCE_SIZE>> custom_forward_list_custom_allocator;
  std::generate_n(std::front_inserter(custom_forward_list_custom_allocator),
                  ALLOCATE_AT_ONCE_SIZE,
                  sequence_generator);
  for(const auto& element : custom_forward_list_custom_allocator)
    out << element << std::endl;
}

}
