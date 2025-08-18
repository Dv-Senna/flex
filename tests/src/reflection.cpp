#include "flex/core/stringifier.hpp"
#include <format>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include <flex/reflection/comparaison.hpp>
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
	flex::reflection::reflection_traits<Office>::member_types,
	std::tuple<
		Address,
		std::vector<Person>,
		const std::vector<Person>,
		flex::WriteOnly<std::vector<Person>>,
		int
	>
>);


struct Foo {
	int a;
	int b;
	struct FlexMetadata {
		static constexpr auto rename = std::make_tuple(
			std::tuple{"c", &Foo::b}
		);
	};
};

static_assert(flex::reflection::userProvided::has_metadata<Foo>);
static_assert(flex::reflection::userProvided::metadata_has_rename<Foo::FlexMetadata>);
static_assert(!flex::reflection::userProvided::metadata_has_remove<Foo::FlexMetadata>);
static_assert(!flex::reflection::userProvided::metadata_has_new_member<Foo::FlexMetadata>);
static_assert(flex::reflection::reflection_traits<Foo>::member_count == 2);
static_assert(std::ranges::equal(
	flex::reflection::reflection_traits<Foo>::member_names,
	std::vector<std::string_view> {"a", "c"}
));
static_assert(std::same_as<
	flex::reflection::reflection_traits<Foo>::member_types,
	std::tuple<int, int>
>);
static_assert(flex::reflection::reflectable<Foo>);


static_assert(flex::reflection::reflectable<Address>);
static_assert(flex::reflection::reflectable<Person>);
static_assert(flex::reflection::aggregate::member_count_v<Address> == 5);
static_assert(flex::reflection::aggregate::member_count_v<Person> == 4);

static_assert(std::same_as<
	flex::reflection::reflection_traits<Address>::member_types,
	std::tuple<std::string, int, int, std::string, std::string>
>);
static_assert(noexcept(
	flex::reflection::foreachNamedMember(std::declval<Address&> (), [](auto&, std::string_view) noexcept {})
));

static_assert(flex::stringifyable<Address>);
static_assert(flex::stringifyable<Person>);
static_assert(flex::nonfailable_stringifyable<Address>);
static_assert(flex::nonfailable_stringifyable<Person>);
static_assert(flex::stringifyable_with<Address, flex::StringifyStyle>);
static_assert(flex::stringifyable_with<Person, flex::StringifyStyle>);


TEST_CASE("reflection_traits", "[reflection]") {
	using namespace std::string_view_literals;
	Address address {};
	flex::reflection::reflection_traits<Address>::getMember<0u> (address) = "Kramgasse";
	flex::reflection::reflection_traits<Address>::getMember<1u> (address) = 49;
	flex::reflection::reflection_traits<Address>::getMember<2u> (address) = 3000;
	flex::reflection::reflection_traits<Address>::getMember<3u> (address) = "Bern";
	flex::reflection::reflection_traits<Address>::getMember<4u> (address) = "Switzerland";

	static_assert(flex::reflection::Comparator<Address>::is_equal_comparable);

	REQUIRE(flex::reflection::equal(address, address));
	REQUIRE(address.street  == "Kramgasse");
	REQUIRE(address.number  == 49);
	REQUIRE(address.code    == 3000);
	REQUIRE(address.town    == "Bern");
	REQUIRE(address.country == "Switzerland");

	Person person {};
	flex::reflection::reflection_traits<Person>::getMember<0u> (person) = "Albert";
	flex::reflection::reflection_traits<Person>::getMember<1u> (person) = "Einstein";
	flex::reflection::reflection_traits<Person>::getMember<2u> (person) = 26;
	flex::reflection::reflection_traits<Person>::getMember<3u> (person) = address;

	REQUIRE(flex::reflection::equal(person, person));
	REQUIRE(person.firstname == "Albert");
	REQUIRE(person.lastname  == "Einstein");
	REQUIRE(person.age       == 26);
	REQUIRE(flex::reflection::equal(person.address, address));

	Office office {};
	flex::reflection::reflection_traits<Office>::getMember<0u> (office).get() = address;
	flex::reflection::reflection_traits<Office>::getMember<1u> (office) = std::vector{person, person};
	flex::reflection::reflection_traits<Office>::getMember<3u> (office) = std::vector{person, person, person};
	flex::reflection::reflection_traits<Office>::getMember<4u> (office).get() = 42;

	REQUIRE(flex::reflection::equal(office, office));
	REQUIRE(flex::reflection::equal(office.mainAddress, address));
	REQUIRE(office.value == 42);
	REQUIRE(flex::reflection::equal(office.m_members, std::vector{person, person, person}));


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
	REQUIRE(flex::toString(address, flex::StringifyStyle{.prettify = true}) == "{\n"
		"    street=Kramgasse,\n"
		"    number=49,\n"
		"    code=3000,\n"
		"    town=Bern,\n"
		"    country=Switzerland\n"
		"}"
	);
	REQUIRE(flex::toString(person, flex::StringifyStyle{.prettify = true}) == "{\n"
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

	REQUIRE(flex::toString(office) ==
		"{address={street=Kramgasse,number=49,code=3000,town=Bern,country="
		"Switzerland},members=[{firstname=Albert,lastname=Einstein,age=26,"
		"address={street=Kramgasse,number=49,code=3000,town=Bern,country="
		"Switzerland}},{firstname=Albert,lastname=Einstein,age=26,address={"
		"street=Kramgasse,number=49,code=3000,town=Bern,country="
		"Switzerland}},{firstname=Albert,lastname=Einstein,age=26,address={"
		"street=Kramgasse,number=49,code=3000,town=Bern,country="
		"Switzerland}}],read-only-members=[{firstname=Albert,lastname="
		"Einstein,age=26,address={street=Kramgasse,number=49,code=3000,"
		"town=Bern,country=Switzerland}},{firstname=Albert,lastname="
		"Einstein,age=26,address={street=Kramgasse,number=49,code=3000,"
		"town=Bern,country=Switzerland}},{firstname=Albert,lastname="
		"Einstein,age=26,address={street=Kramgasse,number=49,code=3000,"
		"town=Bern,country=Switzerland}}],newValue=42}"
	);
	REQUIRE(flex::toString(office, flex::StringifyStyle{.prettify = true}) ==
		"{\n"
		"    address={\n"
		"        street=Kramgasse,\n"
		"        number=49,\n"
		"        code=3000,\n"
		"        town=Bern,\n"
		"        country=Switzerland\n"
		"    },\n"
		"    members=[\n"
		"        {\n"
		"            firstname=Albert,\n"
		"            lastname=Einstein,\n"
		"            age=26,\n"
		"            address={\n"
		"                street=Kramgasse,\n"
		"                number=49,\n"
		"                code=3000,\n"
		"                town=Bern,\n"
		"                country=Switzerland\n"
		"            }\n"
		"        },\n"
		"        {\n"
		"            firstname=Albert,\n"
		"            lastname=Einstein,\n"
		"            age=26,\n"
		"            address={\n"
		"                street=Kramgasse,\n"
		"                number=49,\n"
		"                code=3000,\n"
		"                town=Bern,\n"
		"                country=Switzerland\n"
		"            }\n"
		"        },\n"
		"        {\n"
		"            firstname=Albert,\n"
		"            lastname=Einstein,\n"
		"            age=26,\n"
		"            address={\n"
		"                street=Kramgasse,\n"
		"                number=49,\n"
		"                code=3000,\n"
		"                town=Bern,\n"
		"                country=Switzerland\n"
		"            }\n"
		"        }\n"
		"    ],\n"
		"    read-only-members=[\n"
		"        {\n"
		"            firstname=Albert,\n"
		"            lastname=Einstein,\n"
		"            age=26,\n"
		"            address={\n"
		"                street=Kramgasse,\n"
		"                number=49,\n"
		"                code=3000,\n"
		"                town=Bern,\n"
		"                country=Switzerland\n"
		"            }\n"
		"        },\n"
		"        {\n"
		"            firstname=Albert,\n"
		"            lastname=Einstein,\n"
		"            age=26,\n"
		"            address={\n"
		"                street=Kramgasse,\n"
		"                number=49,\n"
		"                code=3000,\n"
		"                town=Bern,\n"
		"                country=Switzerland\n"
		"            }\n"
		"        },\n"
		"        {\n"
		"            firstname=Albert,\n"
		"            lastname=Einstein,\n"
		"            age=26,\n"
		"            address={\n"
		"                street=Kramgasse,\n"
		"                number=49,\n"
		"                code=3000,\n"
		"                town=Bern,\n"
		"                country=Switzerland\n"
		"            }\n"
		"        }\n"
		"    ],\n"
		"    newValue=42\n"
		"}"
	);
}
