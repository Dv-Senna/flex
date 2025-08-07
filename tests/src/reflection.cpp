#include <format>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include <flex/reflection/reflection.hpp>
#include <flex/reflection/userProvided.hpp>


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

struct Office {
	Address mainAddress;
	int value;
	std::vector<Person> m_members;

	auto getMembers() const noexcept -> const std::vector<Person>& {return m_members;}
	auto setMembers(const std::vector<Person>& members) noexcept -> void {m_members = members;}

	struct FlexMetadata {
		static constexpr auto rename = std::make_tuple(
			std::tuple{"address", &Office::mainAddress}
		);
		static constexpr auto remove = std::make_tuple(
			&Office::value,
			&Office::m_members
		);
		static constexpr auto new_member = std::make_tuple(
			std::tuple{"members", &Office::setMembers, &Office::getMembers},
			std::tuple{"read-only-members", &Office::getMembers},
			std::tuple{"write-only-members", &Office::setMembers},
			std::tuple{"newValue", &Office::value}
		);
	};
};

static_assert(flex::reflection::userProvided::has_metadata<Office>);
static_assert(flex::reflection::userProvided::metadata_has_rename<Office::FlexMetadata>);
static_assert(flex::reflection::userProvided::metadata_has_remove<Office::FlexMetadata>);
static_assert(flex::reflection::userProvided::metadata_has_new_member<Office::FlexMetadata>);
static_assert(flex::reflection::reflection_traits<Office>::member_count == 5);
static_assert(std::ranges::equal(
	flex::reflection::reflection_traits<Office>::member_names,
	std::vector<std::string_view> {"address", "members", "read-only-members", "write-only-members", "newValue"}
));
static_assert(std::same_as<
	flex::reflection::userProvided::get_member_types_t<Office>,
	std::tuple<
		Address,
		std::vector<Person>,
		const std::vector<Person>,
		flex::WriteOnly<std::vector<Person>>,
		int
	>
>);


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

static_assert(flex::stringifyable<Address>);
static_assert(flex::stringifyable<Person>);
static_assert(flex::nonfailable_stringifyable<Address>);
static_assert(flex::nonfailable_stringifyable<Person>);
static_assert(flex::stringifyable_with<Address, flex::reflection::StringifyStyle>);
static_assert(flex::stringifyable_with<Person, flex::reflection::StringifyStyle>);


TEST_CASE("reflection_traits", "[reflection]") {
	using namespace std::string_view_literals;
	Address address {};
	flex::reflection::reflection_traits<Address>::getMember<0u> (address) = "Kramgasse";
	flex::reflection::reflection_traits<Address>::getMember<1u> (address) = 49;
	flex::reflection::reflection_traits<Address>::getMember<2u> (address) = 3000;
	flex::reflection::reflection_traits<Address>::getMember<3u> (address) = "Bern";
	flex::reflection::reflection_traits<Address>::getMember<4u> (address) = "Switzerland";

	Person person {};
	flex::reflection::reflection_traits<Person>::getMember<0u> (person) = "Albert";
	flex::reflection::reflection_traits<Person>::getMember<1u> (person) = "Einstein";
	flex::reflection::reflection_traits<Person>::getMember<2u> (person) = 26;
	flex::reflection::reflection_traits<Person>::getMember<3u> (person) = address;

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

	REQUIRE(flex::toString(address) == "{street=Kramgasse,number=49,code=3000,town=Bern,country=Switzerland}");
	REQUIRE(flex::toString(person) == "{firstname=Albert,lastname=Einstein,age=26,address="
		"{street=Kramgasse,number=49,code=3000,town=Bern,country=Switzerland}}"
	);
	REQUIRE(flex::toString(address, flex::reflection::StringifyStyle{.prettify = true}) == "{\n"
		"    street=Kramgasse,\n"
		"    number=49,\n"
		"    code=3000,\n"
		"    town=Bern,\n"
		"    country=Switzerland\n"
		"}"
	);
	REQUIRE(flex::toString(person, flex::reflection::StringifyStyle{.prettify = true}) == "{\n"
		"    firstname=Albert,\n"
		"    lastname=Einstein,\n"
		"    age=26,\n"
		"    address={\n"
		"        street=Kramgasse,\n"
		"        number=49,\n"
		"        code=3000,\n"
		"        town=Bern,\n"
		"        country=Switzerland\n"
		"    }\n"
		"}"
	);
}
