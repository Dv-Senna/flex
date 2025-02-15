#include <print>

#include <flex/reflection/reflection.hpp>
#include <catch2/catch_test_macros.hpp>

struct Address {
	std::string street;
	int number;
	int postalCode;
	std::string city;
	std::string country;
};

struct Person {
	std::string name;
	std::string surname;
	int age;
	Address address;
};

static_assert(flex::reflection_traits<Person>::HAS_REFLECTION);
static_assert(flex::reflection_traits<Person>::MEMBERS_COUNT == 4);
static_assert(std::get<0> (flex::reflection_traits<Person>::MEMBERS_NAMES) == "name");
static_assert(std::get<1> (flex::reflection_traits<Person>::MEMBERS_NAMES) == "surname");
static_assert(std::get<2> (flex::reflection_traits<Person>::MEMBERS_NAMES) == "age");
static_assert(std::get<3> (flex::reflection_traits<Person>::MEMBERS_NAMES) == "address");
static_assert(std::is_same_v<flex::reflection_traits<Person>::MembersTypes, std::tuple<std::string, std::string, int, Address>>);

static_assert(flex::reflection_traits<Address>::HAS_REFLECTION);
static_assert(flex::reflection_traits<Address>::MEMBERS_COUNT == 5);
static_assert(std::get<0> (flex::reflection_traits<Address>::MEMBERS_NAMES) == "street");
static_assert(std::get<1> (flex::reflection_traits<Address>::MEMBERS_NAMES) == "number");
static_assert(std::get<2> (flex::reflection_traits<Address>::MEMBERS_NAMES) == "postalCode");
static_assert(std::get<3> (flex::reflection_traits<Address>::MEMBERS_NAMES) == "city");
static_assert(std::get<4> (flex::reflection_traits<Address>::MEMBERS_NAMES) == "country");
static_assert(std::is_same_v<flex::reflection_traits<Address>::MembersTypes, std::tuple<std::string, int, int, std::string, std::string>>);




TEST_CASE("reflection", "[reflection]") {
	std::println("Address<0> : '{}'", std::get<0> (flex::reflection_traits<Address>::MEMBERS_NAMES));
	std::println("Address<1> : '{}'", std::get<1> (flex::reflection_traits<Address>::MEMBERS_NAMES));
	std::println("Address<2> : '{}'", std::get<2> (flex::reflection_traits<Address>::MEMBERS_NAMES));
	std::println("Address<3> : '{}'", std::get<3> (flex::reflection_traits<Address>::MEMBERS_NAMES));
	std::println("Address<4> : '{}'", std::get<4> (flex::reflection_traits<Address>::MEMBERS_NAMES));

	std::println("Person<0> : '{}'", std::get<0> (flex::reflection_traits<Person>::MEMBERS_NAMES));
	std::println("Person<1> : '{}'", std::get<1> (flex::reflection_traits<Person>::MEMBERS_NAMES));
	std::println("Person<2> : '{}'", std::get<2> (flex::reflection_traits<Person>::MEMBERS_NAMES));
	std::println("Person<3> : '{}'", std::get<3> (flex::reflection_traits<Person>::MEMBERS_NAMES));

	REQUIRE(1 == 1);
}
