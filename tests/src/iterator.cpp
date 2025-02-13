#include <list>
#include <print>

#include <flex/contiguousIterator.hpp>
#include <flex/iterator.hpp>

#include <catch2/catch_test_macros.hpp>


static_assert(flex::is_input_iterator_v<std::list<int>::iterator>);
static_assert(flex::is_forward_iterator_v<std::list<int>::iterator>);
static_assert(flex::is_bidirectional_iterator_v<std::list<int>::iterator>);
static_assert(!flex::is_random_access_iterator_v<std::list<int>::iterator>);
static_assert(!flex::is_contiguous_iterator_v<std::list<int>::iterator>);

static_assert(flex::is_input_iterator_v<std::vector<int>::iterator>);
static_assert(flex::is_forward_iterator_v<std::vector<int>::iterator>);
static_assert(flex::is_bidirectional_iterator_v<std::vector<int>::iterator>);
static_assert(flex::is_random_access_iterator_v<std::vector<int>::iterator>);
static_assert(!flex::is_contiguous_iterator_v<std::vector<int>::iterator>);

static_assert(flex::is_input_iterator_v<flex::ContiguousIterator<int, int>>);
static_assert(flex::is_forward_iterator_v<flex::ContiguousIterator<int, int>>);
static_assert(flex::is_bidirectional_iterator_v<flex::ContiguousIterator<int, int>>);
static_assert(flex::is_random_access_iterator_v<flex::ContiguousIterator<int, int>>);
static_assert(flex::is_contiguous_iterator_v<flex::ContiguousIterator<int, int>>);


TEST_CASE("std::list<int>", "[iterator]") {
	std::list<int> list {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	
	std::println("{}", (std::uint32_t)flex::iterator_traits<decltype(list.begin())>::CATEGORY);

	REQUIRE(flex::distance(list.begin(), list.end()) == 10);
}
