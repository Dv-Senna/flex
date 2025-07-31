#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include <flex/reflection/reflection.hpp>


struct Address {
	std::string street;
	int number;
	int code;
	std::string town;
	std::string country;
};

struct Person {
	std::string firstname;
	std::string lastname;
	int age;
	Address address;
};


static_assert(flex::reflection::reflectable<Address>);
static_assert(flex::reflection::reflectable<Person>);
static_assert(flex::reflection::aggregate::member_count_v<Address> == 5);
static_assert(flex::reflection::aggregate::member_count_v<Person> == 4);

static_assert(std::same_as<
	flex::reflection::reflection_traits<Address>::member_types,
	std::tuple<std::string, int, int, std::string, std::string>
>);

static_assert(!noexcept(
	flex::reflection::foreachNamedMember(std::declval<Address&> (), [](auto&, std::string_view) {})
));
static_assert(noexcept(
	flex::reflection::foreachNamedMember(std::declval<Address&> (), [](auto&, std::string_view) noexcept {})
));



TEST_CASE("reflection_traits", "[reflection]") {
	using namespace std::string_view_literals;
	Address address {};
	flex::reflection::reflection_traits<Address>::getMember<0u> (address) = "Kramgasse";
	flex::reflection::reflection_traits<Address>::getMember<1u> (address) = 49;
	flex::reflection::reflection_traits<Address>::getMember<2u> (address) = 3000;
	flex::reflection::reflection_traits<Address>::getMember<3u> (address) = "Bern";
	flex::reflection::reflection_traits<Address>::getMember<4u> (address) = "Switzerland";

	REQUIRE(flex::reflection::reflection_traits<Address>::name == "Address");
	std::size_t i {};
	flex::reflection::foreachNamedMember(address, [&i](auto& member, std::string_view name) {
		static const std::array expected {
			"street=Kramgasse"sv,
			"number=49"sv,
			"code=3000"sv,
			"town=Bern"sv,
			"country=Switzerland"sv
		};

		REQUIRE(std::format("{}={}", name, member) == expected[i++]);
	});
}
