#include <flex/reflection/enums.hpp>
#include <flex/bitfield.hpp>

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


#ifndef __cpp_impl_reflection
	FLEX_SET_MAX_ENUM_SIZE(SomeBitfield, 4);
	FLEX_MAKE_ENUM_BITFLAG(SomeBitfield);
#endif


TEST_CASE("macros", "[reflection]") {
	REQUIRE(flex::toString((SomeEnum)0).value_or("<invalid>") == "eA");
	REQUIRE(flex::toString((SomeEnum)1).value_or("<invalid>") == "eB");
	REQUIRE(flex::toString((SomeEnum)2).value_or("<invalid>") == "eC");
	REQUIRE(!flex::toString((SomeEnum)3));

	REQUIRE(flex::toString((SomeBitfield)0b001).value_or("<invalid>") == "eREAD");
	REQUIRE(flex::toString((SomeBitfield)0b010).value_or("<invalid>") == "eWRITE");
	REQUIRE(flex::toString((SomeBitfield)0b100).value_or("<invalid>") == "eEXEC");

	flex::Bitfield<SomeBitfield> bitfield {SomeBitfield::eREAD | SomeBitfield::eEXEC};
	REQUIRE(bitfield & SomeBitfield::eREAD);
	REQUIRE(!(bitfield & SomeBitfield::eWRITE));
	REQUIRE(bitfield & SomeBitfield::eEXEC);
	REQUIRE(flex::toString(bitfield).value_or("<invalid>") == "eREAD | eEXEC");
	REQUIRE(!flex::toString(flex::Bitfield<SomeBitfield> {}));
}
