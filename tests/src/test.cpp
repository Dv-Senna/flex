#include <flex/test.hpp>
#include <flex/config.hpp>
#include <flex/contiguousIterator.hpp>
#include <flex/typeTraits.hpp>
#include <flex/iterator.hpp>

#include <catch2/catch_test_macros.hpp>


TEST_CASE("test", "[test]") {
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

	flex::sayHello();
	REQUIRE(1 == 1);
}
