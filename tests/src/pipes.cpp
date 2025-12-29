#include <print>

#include <catch2/catch_test_macros.hpp>

#include <flex/pipes/outcome_traits.hpp>


TEST_CASE("outcome_traits", "[pipes]") {
	using Traits = flex::pipes::outcome_traits<std::optional<int>>;
	static_assert(std::same_as<Traits::type, std::optional<int>>);
	static_assert(std::same_as<Traits::value_type, int>);
	static_assert(std::same_as<Traits::error_type, void>);

	std::optional<int> nullopt {std::nullopt};
	std::optional<int> opt {12};

	REQUIRE(!Traits::has_value(nullopt));
	REQUIRE(Traits::has_value(opt));
	REQUIRE(Traits::value(opt) == 12);
	REQUIRE(Traits::value_or(nullopt, -1) == -1);
	REQUIRE(Traits::value_or(opt, -1) == 12);
}
