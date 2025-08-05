#include <algorithm>
#include <ranges>

#include <catch2/catch_test_macros.hpp>

#include <flex/containers/inplaceVector.hpp>


TEST_CASE("inplace-vector", "[containers]") {
	constexpr std::size_t capacity {16};
	flex::containers::InplaceVector<std::size_t, capacity> inplaceVector {};
	REQUIRE(inplaceVector.empty());

	for (const auto i : std::views::iota(std::size_t{0}, capacity)) {
		REQUIRE(inplaceVector.tryPushBack(i));
		REQUIRE(inplaceVector.size() == i + 1u);
		REQUIRE(inplaceVector[i] == i);
		REQUIRE(inplaceVector[-1] == i);
	}

	REQUIRE(!inplaceVector.tryPushBack(capacity));
	REQUIRE(inplaceVector.size() == capacity);
	REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, capacity)));
	REQUIRE(inplaceVector.end() - inplaceVector.begin() == capacity);
	REQUIRE(inplaceVector.cend() - inplaceVector.cbegin() == capacity);


	SECTION("initialization") {
		{
			flex::containers::InplaceVector<std::size_t, capacity> inplaceVector {0u, 1u, 2u, 3u, 4u, 5u};
			REQUIRE(inplaceVector.size() == 6);
			REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, std::size_t{6})));
		}

		{
			auto range {std::views::iota(std::size_t{0}, capacity)};
			flex::containers::InplaceVector<std::size_t, capacity> inplaceVector {
				std::ranges::begin(range),
				std::ranges::end(range)
			};
			REQUIRE(inplaceVector.size() == capacity);
			REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, capacity)));
		}

	#if defined(__cpp_lib_containers_ranges) && __cpp_lib_containers_ranges >= 202202L
		{
			flex::containers::InplaceVector<std::size_t, capacity> inplaceVector {
				std::from_range,
				std::views::iota(std::size_t{0}, capacity)
			};
			REQUIRE(inplaceVector.size() == capacity);
			REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, capacity)));
			flex::containers::InplaceVector<std::size_t, capacity> inplaceVector2 {
				std::from_range,
				inplaceVector
			};
			REQUIRE(inplaceVector2.size() == capacity);
			REQUIRE(std::ranges::equal(inplaceVector2, std::views::iota(std::size_t{0}, capacity)));
		}

		{
			auto inplaceVector {std::views::iota(std::size_t{0}, capacity)
				| std::ranges::to<flex::containers::InplaceVector<std::size_t, capacity>> ()
			};
			REQUIRE(inplaceVector.size() == capacity);
			REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, capacity)));
		}
	#endif

		{
			auto inplaceVector {flex::containers::makeInplaceVector<capacity> (std::size_t{0}, 1, 2, 3, 4, 5)};
			REQUIRE(inplaceVector.size() == 6);
			REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, std::size_t{6})));
		}
	}

	SECTION("copy-move") {
		flex::containers::InplaceVector<std::size_t, capacity> inplaceVector2 {inplaceVector};
		REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector2, std::views::iota(std::size_t{0}, capacity)));

		flex::containers::InplaceVector<std::size_t, capacity> inplaceVector3 {};
		inplaceVector3 = inplaceVector;
		REQUIRE(std::ranges::equal(inplaceVector, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector2, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector3, std::views::iota(std::size_t{0}, capacity)));

		flex::containers::InplaceVector<std::size_t, capacity> inplaceVector4 {std::move(inplaceVector)};
		REQUIRE(inplaceVector.empty());
		REQUIRE(std::ranges::equal(inplaceVector2, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector3, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector4, std::views::iota(std::size_t{0}, capacity)));

		inplaceVector = inplaceVector2;
		flex::containers::InplaceVector<std::size_t, capacity> inplaceVector5 {};
		inplaceVector5 = std::move(inplaceVector);
		REQUIRE(inplaceVector.empty());
		REQUIRE(std::ranges::equal(inplaceVector2, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector3, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector4, std::views::iota(std::size_t{0}, capacity)));
		REQUIRE(std::ranges::equal(inplaceVector5, std::views::iota(std::size_t{0}, capacity)));
	}
}
