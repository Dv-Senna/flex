#include <flex/test.hpp>
#include <flex/config.hpp>
#include <flex/contiguousIterator.hpp>
#include <flex/typeTraits.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("test", "[test]") {
	flex::sayHello();
	REQUIRE(1 == 1);
}
