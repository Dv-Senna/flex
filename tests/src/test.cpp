#include <flex/test.hpp>
#include <flex/config.hpp>
#include <flex/contiguousIterator.hpp>
#include <flex/typeTraits.hpp>
#include <flex/iterator.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("test", "[test]") {
	static_assert(flex::is_iterator_category_v<std::vector<int>::iterator, flex::iteratorCategory::Input>);
	static_assert(flex::is_iterator_category_v<std::vector<int>::iterator, flex::iteratorCategory::Forward>);
	static_assert(flex::is_iterator_category_v<std::vector<int>::iterator, flex::iteratorCategory::Bidirectional>);
	static_assert(flex::is_iterator_category_v<std::vector<int>::iterator, flex::iteratorCategory::RandomAccess>);
	static_assert(!flex::is_iterator_category_v<std::vector<int>::iterator, flex::iteratorCategory::Contiguous>);

	flex::sayHello();
	REQUIRE(1 == 1);
}
