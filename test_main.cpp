#include "version.h"
#include "utils.h"
#include "custom_allocator.h"
#include "custom_forward_list.h"
#include "homework_3.h"
#include <map>

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_version)

BOOST_AUTO_TEST_CASE(test_version_valid)
{
  BOOST_CHECK( version() > version_info() );
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(test_suite_factorial)

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

  //BOOST_STATIC_ASSERT(1 == factorial(1.1));   // compilation error
  BOOST_CHECK_THROW(factorial(-1), std::invalid_argument);
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

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(test_suite_custom_allocator)

static auto malloc_call_count{0};
void* custom_malloc (size_t size)
{
  ++malloc_call_count;
  return std::malloc(size);
};

BOOST_AUTO_TEST_CASE(test_custom_allocator_less_allocations)
{
  auto pair_generator = [i=0] () mutable {
    auto value = std::make_pair(i, factorial(i));
    ++i;
    return value;
  };

  const auto allocate_block_size{10};
  std::map<int, int, std::less<int>, custom_allocator<std::pair<const int, int>, allocate_block_size, &custom_malloc>> map_custom_allocator;
  
  malloc_call_count = 0;
  std::generate_n(std::inserter(map_custom_allocator, std::begin(map_custom_allocator)),
                  allocate_block_size,
                  pair_generator);

  BOOST_CHECK(1 == malloc_call_count);
  map_custom_allocator[allocate_block_size] = allocate_block_size;
  BOOST_CHECK(2 == malloc_call_count);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(test_suite_custom_forward_list)

BOOST_AUTO_TEST_CASE(test_custom_forward_list_empty)
{
  custom_forward_list<int> test_container_1;
  BOOST_CHECK(true == test_container_1.empty());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_push_front)
{
  custom_forward_list<uint64_t> test_container_1;
  test_container_1.push_front(0xDEADBEEF);
  BOOST_CHECK(false == test_container_1.empty());
  BOOST_CHECK(0xDEADBEEF == test_container_1.front());
  test_container_1.push_front(0xABADBABE);
  BOOST_CHECK(false == test_container_1.empty());
  BOOST_CHECK(0xABADBABE == test_container_1.front());
}

BOOST_AUTO_TEST_SUITE_END()



struct initialized_one_list
{
  initialized_one_list()
  {
    test_container_1.push_front(0xDEADBEEF);
    test_container_1.push_front(0xABADBABE);
  }

  custom_forward_list<uint64_t> test_container_1;
};

BOOST_FIXTURE_TEST_SUITE(fixture_test_suite_custom_forward_list, initialized_one_list)

BOOST_AUTO_TEST_CASE(test_custom_forward_list_pop_front)
{
  BOOST_CHECK(0xABADBABE == test_container_1.front());
  test_container_1.pop_front();
  BOOST_CHECK(0xDEADBEEF == test_container_1.front());
  test_container_1.pop_front();
  BOOST_CHECK(true == test_container_1.empty());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_size)
{
  BOOST_CHECK(2 == test_container_1.size());
  test_container_1.pop_front();
  BOOST_CHECK(1 == test_container_1.size());
  test_container_1.pop_front();
  BOOST_CHECK(0 == test_container_1.size());

  decltype(test_container_1) test_container_2;
  BOOST_CHECK(0 == test_container_2.size());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_clear)
{
  BOOST_CHECK(false == test_container_1.empty());
  test_container_1.clear();
  BOOST_CHECK(true == test_container_1.empty());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_iterator)
{
  BOOST_CHECK(2 == std::distance(std::cbegin(test_container_1), std::cend(test_container_1)));
  auto itr = std::cbegin(test_container_1);
  BOOST_CHECK(0xABADBABE == *itr);
  ++itr;
  BOOST_CHECK(0xDEADBEEF == *itr);
  ++itr;
  BOOST_CHECK(itr == std::cend(test_container_1));
  test_container_1.clear();
  BOOST_CHECK(0 == std::distance(std::cbegin(test_container_1), std::cend(test_container_1)));
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_equality)
{
  decltype(test_container_1) test_container_2;
  test_container_2.push_front(0xDEADBEEF);
  BOOST_CHECK(test_container_1 != test_container_2);
  test_container_2.push_front(0xABADBABE);
  BOOST_CHECK(test_container_1 == test_container_2);
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_copy_ctr)
{
  decltype(test_container_1) test_container_2{test_container_1};

  BOOST_CHECK(false == test_container_1.empty());
  BOOST_CHECK(false == test_container_2.empty());
  BOOST_CHECK(test_container_1.front() == test_container_2.front());
  BOOST_CHECK(&test_container_1.front() != &test_container_2.front());
  test_container_1.pop_front();
  test_container_2.pop_front();
  BOOST_CHECK(test_container_1.front() == test_container_2.front());
  BOOST_CHECK(&test_container_1.front() != &test_container_2.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_assign)
{
  decltype(test_container_1) test_container_2;
  test_container_2 = test_container_1;

  BOOST_CHECK(false == test_container_1.empty());
  BOOST_CHECK(false == test_container_2.empty());
  BOOST_CHECK(test_container_1.front() == test_container_2.front());
  BOOST_CHECK(&test_container_1.front() != &test_container_2.front());
  test_container_1.pop_front();
  test_container_2.pop_front();
  BOOST_CHECK(test_container_1.front() == test_container_2.front());
  BOOST_CHECK(&test_container_1.front() != &test_container_2.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_move_ctr)
{
  auto itr = std::cbegin(test_container_1);
  auto address_1 =& *itr;
  ++itr;
  auto address_2 =& *itr;

  decltype(test_container_1) test_container_2{std::move(test_container_1)};

  BOOST_CHECK(true == test_container_1.empty());
  BOOST_CHECK(false == test_container_2.empty());
  BOOST_CHECK(0xABADBABE == test_container_2.front());
  BOOST_CHECK(address_1 == &test_container_2.front());
  test_container_2.pop_front();
  BOOST_CHECK(0xDEADBEEF == test_container_2.front());
  BOOST_CHECK(address_2 == &test_container_2.front());
}

BOOST_AUTO_TEST_CASE(test_custom_forward_list_move_asign)
{
  auto itr = std::cbegin(test_container_1);
  auto address_1 =& *itr;
  ++itr;
  auto address_2 =& *itr;

  decltype(test_container_1) test_container_2;
  test_container_2 = std::move(test_container_1);
  
  BOOST_CHECK(true == test_container_1.empty());
  BOOST_CHECK(false == test_container_2.empty());
  BOOST_CHECK(0xABADBABE == test_container_2.front());
  BOOST_CHECK(address_1 == &test_container_2.front());
  test_container_2.pop_front();
  BOOST_CHECK(0xDEADBEEF == test_container_2.front());
  BOOST_CHECK(address_2 == &test_container_2.front());
}

struct initialized_two_lists : initialized_one_list
{
  initialized_two_lists()
  {
    test_container_2.push_front(0x55555555);
    test_container_2.push_front(0x77777777);
    test_container_2.push_front(0x99999999);
  }

  custom_forward_list<uint64_t> test_container_2;
};

BOOST_FIXTURE_TEST_CASE(test_custom_forward_list_swap, initialized_two_lists)
{
  BOOST_CHECK(2 == test_container_1.size());
  BOOST_CHECK(3 == test_container_2.size());

  test_container_1.swap(test_container_2);

  BOOST_CHECK(3 == test_container_1.size());
  BOOST_CHECK(2 == test_container_2.size());
  BOOST_CHECK(0x99999999 == test_container_1.front());
  BOOST_CHECK(0xABADBABE == test_container_2.front());
  test_container_1.pop_front();
  test_container_2.pop_front();
  BOOST_CHECK(0x77777777 == test_container_1.front());
  BOOST_CHECK(0xDEADBEEF == test_container_2.front());
  test_container_1.pop_front();
  test_container_2.pop_front();
  BOOST_CHECK(0x55555555 == test_container_1.front());
}

BOOST_FIXTURE_TEST_CASE(test_custom_forward_list_swap_global_funtion, initialized_two_lists)
{
  BOOST_CHECK(2 == test_container_1.size());
  BOOST_CHECK(3 == test_container_2.size());

  swap(test_container_1, test_container_2);
  
  BOOST_CHECK(3 == test_container_1.size());
  BOOST_CHECK(2 == test_container_2.size());
  BOOST_CHECK(0x99999999 == test_container_1.front());
  BOOST_CHECK(0xABADBABE == test_container_2.front());
  test_container_1.pop_front();
  test_container_2.pop_front();
  BOOST_CHECK(0x77777777 == test_container_1.front());
  BOOST_CHECK(0xDEADBEEF == test_container_2.front());
  test_container_1.pop_front();
  test_container_2.pop_front();
  BOOST_CHECK(0x55555555 == test_container_1.front());
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(test_suite_homework)

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
