#include "version.h"
#include "utils.h"
#include "custom_allocator.h"
#include "custom_forward_list.h"
#include "homework_3.h"
#include <map>

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_main)

BOOST_AUTO_TEST_CASE(test_version_valid)
{
  BOOST_CHECK( version() > version_info() );
}

BOOST_AUTO_TEST_CASE(test_factorial_function)
{
  BOOST_STATIC_ASSERT(1 == factorial(0));
  BOOST_STATIC_ASSERT(1 == factorial(1));
  BOOST_STATIC_ASSERT(2 == factorial(2));
  BOOST_STATIC_ASSERT(6 == factorial(3));
  BOOST_STATIC_ASSERT(24 == factorial(4));
  BOOST_STATIC_ASSERT(120 == factorial(5));
  BOOST_STATIC_ASSERT(720 == factorial(6));
  BOOST_STATIC_ASSERT(5040 == factorial(7));
  BOOST_STATIC_ASSERT(40320 == factorial(8));
  BOOST_STATIC_ASSERT(362880 == factorial(9));
}

BOOST_AUTO_TEST_CASE(test_factorial_template)
{
  BOOST_STATIC_ASSERT(1 == factorial_t<0>::value);
  BOOST_STATIC_ASSERT(1 == factorial_t<1>::value);
  BOOST_STATIC_ASSERT(2 == factorial_t<2>::value);
  BOOST_STATIC_ASSERT(6 == factorial_t<3>::value);
  BOOST_STATIC_ASSERT(24 == factorial_t<4>::value);
  BOOST_STATIC_ASSERT(120 == factorial_t<5>::value);
  BOOST_STATIC_ASSERT(720 == factorial_t<6>::value);
  BOOST_STATIC_ASSERT(5040 == factorial_t<7>::value);
  BOOST_STATIC_ASSERT(40320 == factorial_t<8>::value);
  BOOST_STATIC_ASSERT(362880 == factorial_t<9>::value);
}

BOOST_AUTO_TEST_CASE(test_custom_allocator_continuous_allocation)
{
  const uint8_t default_filler{0xAA};
  const uint8_t magic_filler{0xDD};
  const auto allocate_block_size{10};

  struct magic_word_class
  {
    magic_word_class() {
      memset(&magic_word, magic_filler, sizeof(magic_word));
    }
    uint64_t magic_word;
  };

  const auto bytes_sequence_count{sizeof(magic_word_class) * allocate_block_size};

  std::array<magic_word_class*, allocate_block_size> pointers;
  custom_allocator<magic_word_class, allocate_block_size> allocator;

  pointers[0] = allocator.allocate(1);
  auto begin = reinterpret_cast<uint8_t*>(pointers[0]);

  memset(begin, default_filler, bytes_sequence_count);
  BOOST_CHECK(bytes_sequence_count == std::count(begin, begin + bytes_sequence_count, default_filler));

  allocator.construct(pointers[0]);
  for(auto i = 1; i < allocate_block_size; ++i)
  {
    pointers[i] = allocator.allocate(1);
    allocator.construct(pointers[i]);
  }
  
  BOOST_CHECK(bytes_sequence_count == std::count(begin, begin + bytes_sequence_count, magic_filler));
}

static auto malloc_call_count{0};
void* custom_malloc (size_t size)
{
  ++malloc_call_count;
  return std::malloc(size);
};

BOOST_AUTO_TEST_CASE(test_custom_allocator_less_allocations)
{
  const auto allocate_block_size{10};
  std::map<int, int, std::less<int>, custom_allocator<std::pair<const int, int>, allocate_block_size, &custom_malloc>> map_custom_allocator;
  malloc_call_count = 0;

  for (int i = 0; i < allocate_block_size; ++i)
    map_custom_allocator[i] = i;

  BOOST_CHECK(1 == malloc_call_count);
  map_custom_allocator[allocate_block_size] = allocate_block_size;
  BOOST_CHECK(2 == malloc_call_count);
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_empty)
{
  custom_forward_list<int> fwd_list_default_allocator;
  BOOST_CHECK(true == fwd_list_default_allocator.empty());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_push_front)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  BOOST_CHECK(false == fwd_list_default_allocator.empty());
  BOOST_CHECK(0xDEADBEEF == fwd_list_default_allocator.front());
  fwd_list_default_allocator.push_front(0xABADBABE);
  BOOST_CHECK(false == fwd_list_default_allocator.empty());
  BOOST_CHECK(0xABADBABE == fwd_list_default_allocator.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_pop_front)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);
  BOOST_CHECK(false == fwd_list_default_allocator.empty());
  BOOST_CHECK(0xABADBABE == fwd_list_default_allocator.front());
  fwd_list_default_allocator.pop_front();
  BOOST_CHECK(false == fwd_list_default_allocator.empty());
  BOOST_CHECK(0xDEADBEEF == fwd_list_default_allocator.front());
  fwd_list_default_allocator.pop_front();
  BOOST_CHECK(true == fwd_list_default_allocator.empty());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_size)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  BOOST_CHECK(0 == fwd_list_default_allocator.size());
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  BOOST_CHECK(1 == fwd_list_default_allocator.size());
  fwd_list_default_allocator.push_front(0xABADBABE);
  BOOST_CHECK(2 == fwd_list_default_allocator.size());
  fwd_list_default_allocator.pop_front();
  BOOST_CHECK(1 == fwd_list_default_allocator.size());
  fwd_list_default_allocator.pop_front();
  BOOST_CHECK(0 == fwd_list_default_allocator.size());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_clear)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);
  BOOST_CHECK(false == fwd_list_default_allocator.empty());
  fwd_list_default_allocator.clear();
  BOOST_CHECK(true == fwd_list_default_allocator.empty());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_iterator)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  BOOST_CHECK(0 == std::distance(fwd_list_default_allocator.begin(), fwd_list_default_allocator.end()));
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);
  BOOST_CHECK(2 == std::distance(fwd_list_default_allocator.begin(), fwd_list_default_allocator.end()));
  auto itr = fwd_list_default_allocator.begin();
  BOOST_CHECK(0xABADBABE == *itr);
  ++itr;
  BOOST_CHECK(0xDEADBEEF == *itr);
  ++itr;
  BOOST_CHECK(itr == fwd_list_default_allocator.end());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_equal)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);

  custom_forward_list<uint64_t> fwd_list_default_allocator_2;
  fwd_list_default_allocator_2.push_front(0xDEADBEEF);
  fwd_list_default_allocator_2.push_front(0xABADBABE);

  BOOST_CHECK(fwd_list_default_allocator == fwd_list_default_allocator_2);
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_not_equal)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);

  custom_forward_list<uint64_t> fwd_list_default_allocator_2;
  fwd_list_default_allocator_2.push_front(0xDEADBEEF);

  BOOST_CHECK(fwd_list_default_allocator != fwd_list_default_allocator_2);
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_copy_ctr)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);

  custom_forward_list<uint64_t> fwd_list_default_allocator_2{fwd_list_default_allocator};

  BOOST_CHECK(false == fwd_list_default_allocator.empty());
  BOOST_CHECK(false == fwd_list_default_allocator_2.empty());
  BOOST_CHECK(fwd_list_default_allocator.front() == fwd_list_default_allocator_2.front());
  BOOST_CHECK(&fwd_list_default_allocator.front() != &fwd_list_default_allocator_2.front());
  fwd_list_default_allocator.pop_front();
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(fwd_list_default_allocator.front() == fwd_list_default_allocator_2.front());
  BOOST_CHECK(&fwd_list_default_allocator.front() != &fwd_list_default_allocator_2.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_assign)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);

  custom_forward_list<uint64_t> fwd_list_default_allocator_2;
  fwd_list_default_allocator_2 = fwd_list_default_allocator;

  BOOST_CHECK(false == fwd_list_default_allocator.empty());
  BOOST_CHECK(false == fwd_list_default_allocator_2.empty());
  BOOST_CHECK(fwd_list_default_allocator.front() == fwd_list_default_allocator_2.front());
  BOOST_CHECK(&fwd_list_default_allocator.front() != &fwd_list_default_allocator_2.front());
  fwd_list_default_allocator.pop_front();
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(fwd_list_default_allocator.front() == fwd_list_default_allocator_2.front());
  BOOST_CHECK(&fwd_list_default_allocator.front() != &fwd_list_default_allocator_2.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_move_ctr)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  auto address_1 =& fwd_list_default_allocator.front();
  fwd_list_default_allocator.push_front(0xABADBABE);
  auto address_2 =& fwd_list_default_allocator.front();

  custom_forward_list<uint64_t> fwd_list_default_allocator_2{std::move(fwd_list_default_allocator)};

  BOOST_CHECK(true == fwd_list_default_allocator.empty());
  BOOST_CHECK(false == fwd_list_default_allocator_2.empty());
  BOOST_CHECK(0xABADBABE == fwd_list_default_allocator_2.front());
  BOOST_CHECK(address_2 == &fwd_list_default_allocator_2.front());
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(0xDEADBEEF == fwd_list_default_allocator_2.front());
  BOOST_CHECK(address_1 == &fwd_list_default_allocator_2.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_move_asign)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  auto address_1 =& fwd_list_default_allocator.front();
  fwd_list_default_allocator.push_front(0xABADBABE);
  auto address_2 =& fwd_list_default_allocator.front();

  custom_forward_list<uint64_t> fwd_list_default_allocator_2;
  fwd_list_default_allocator_2 = std::move(fwd_list_default_allocator);
  
  BOOST_CHECK(true == fwd_list_default_allocator.empty());
  BOOST_CHECK(false == fwd_list_default_allocator_2.empty());
  BOOST_CHECK(0xABADBABE == fwd_list_default_allocator_2.front());
  BOOST_CHECK(address_2 == &fwd_list_default_allocator_2.front());
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(0xDEADBEEF == fwd_list_default_allocator_2.front());
  BOOST_CHECK(address_1 == &fwd_list_default_allocator_2.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_swap)
{
  custom_forward_list<uint64_t> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);

  custom_forward_list<uint64_t> fwd_list_default_allocator_2;
  fwd_list_default_allocator_2.push_front(0x55555555);
  fwd_list_default_allocator_2.push_front(0x77777777);
  fwd_list_default_allocator_2.push_front(0x99999999);

  BOOST_CHECK(2 == fwd_list_default_allocator.size());
  BOOST_CHECK(3 == fwd_list_default_allocator_2.size());

  fwd_list_default_allocator.swap(fwd_list_default_allocator_2);

  BOOST_CHECK(3 == fwd_list_default_allocator.size());
  BOOST_CHECK(2 == fwd_list_default_allocator_2.size());
  BOOST_CHECK(0x99999999 == fwd_list_default_allocator.front());
  BOOST_CHECK(0xABADBABE == fwd_list_default_allocator_2.front());
  fwd_list_default_allocator.pop_front();
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(0x77777777 == fwd_list_default_allocator.front());
  BOOST_CHECK(0xDEADBEEF == fwd_list_default_allocator_2.front());
  fwd_list_default_allocator.pop_front();
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(0x55555555 == fwd_list_default_allocator.front());
  fwd_list_default_allocator.pop_front();
  BOOST_CHECK(true == fwd_list_default_allocator.empty());
  BOOST_CHECK(true == fwd_list_default_allocator_2.empty());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_swap_global_funtion)
{
  custom_forward_list<uint64_t, custom_allocator<uint64_t, 10>> fwd_list_default_allocator;
  fwd_list_default_allocator.push_front(0xDEADBEEF);
  fwd_list_default_allocator.push_front(0xABADBABE);

  custom_forward_list<uint64_t, custom_allocator<uint64_t, 10>> fwd_list_default_allocator_2;
  fwd_list_default_allocator_2.push_front(0x55555555);
  fwd_list_default_allocator_2.push_front(0x77777777);
  fwd_list_default_allocator_2.push_front(0x99999999);

  BOOST_CHECK(2 == fwd_list_default_allocator.size());
  BOOST_CHECK(3 == fwd_list_default_allocator_2.size());

  swap(fwd_list_default_allocator, fwd_list_default_allocator_2);
  
  BOOST_CHECK(3 == fwd_list_default_allocator.size());
  BOOST_CHECK(2 == fwd_list_default_allocator_2.size());
  BOOST_CHECK(0x99999999 == fwd_list_default_allocator.front());
  BOOST_CHECK(0xABADBABE == fwd_list_default_allocator_2.front());
  fwd_list_default_allocator.pop_front();
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(0x77777777 == fwd_list_default_allocator.front());
  BOOST_CHECK(0xDEADBEEF == fwd_list_default_allocator_2.front());
  fwd_list_default_allocator.pop_front();
  fwd_list_default_allocator_2.pop_front();
  BOOST_CHECK(0x55555555 == fwd_list_default_allocator.front());
  fwd_list_default_allocator.pop_front();
  BOOST_CHECK(true == fwd_list_default_allocator.empty());
  BOOST_CHECK(true == fwd_list_default_allocator_2.empty());
}

BOOST_AUTO_TEST_CASE(test_homework_3_valid)
{
  std::string result{
    "0 1\n"
    "1 1\n"
    "2 2\n"
    "3 6\n"
    "4 24\n"
    "5 120\n"
    "6 720\n"
    "7 5040\n"
    "8 40320\n"
    "9 362880\n"
    "9\n"
    "8\n"
    "7\n"
    "6\n"
    "5\n"
    "4\n"
    "3\n"
    "2\n"
    "1\n"
    "0\n"
  };
  std::ostringstream oss;

  homework_3(oss);
  BOOST_CHECK_EQUAL(oss.str(), result);
}

BOOST_AUTO_TEST_SUITE_END()
