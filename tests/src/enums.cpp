#include <flex/reflection/enums.hpp>

#include <catch2/catch_test_macros.hpp>

enum class SomeEnum {
	eA,
	eB,
	eC
};


enum class SomeBitfield {
	eREAD = 0b001,
	eWRITE = 0b010,
	eEXEC = 0b100,
};


FLEX_SET_MAX_ENUM_SIZE(SomeBitfield, 3);
FLEX_MAKE_ENUM_BITFLAG(SomeBitfield);


TEST_CASE("macros", "[reflection]") {
	REQUIRE(*flex::toString((SomeEnum)0) == "eA");
	REQUIRE(*flex::toString((SomeEnum)1) == "eB");
	REQUIRE(*flex::toString((SomeEnum)2) == "eC");
	REQUIRE(!flex::toString((SomeEnum)3));

	REQUIRE(*flex::toString((SomeBitfield)0b001) == "eREAD");
	REQUIRE(*flex::toString((SomeBitfield)0b010) == "eWRITE");
	REQUIRE(*flex::toString((SomeBitfield)0b100) == "eEXEC");
}
