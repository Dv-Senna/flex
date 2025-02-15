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

static_assert(flex::is_reflectable_v<Person>);
static_assert(flex::reflection_members_count_v<Person> == 4);
static_assert(std::get<0> (flex::reflection_members_names_v<Person>) == "name");
static_assert(std::get<1> (flex::reflection_members_names_v<Person>) == "surname");
static_assert(std::get<2> (flex::reflection_members_names_v<Person>) == "age");
static_assert(std::get<3> (flex::reflection_members_names_v<Person>) == "address");
static_assert(std::is_same_v<flex::reflection_members_t<Person>, std::tuple<std::string, std::string, int, Address>>);

static_assert(flex::is_reflectable_v<Address>);
static_assert(flex::reflection_members_count_v<Address> == 5);
static_assert(std::get<0> (flex::reflection_members_names_v<Address>) == "street");
static_assert(std::get<1> (flex::reflection_members_names_v<Address>) == "number");
static_assert(std::get<2> (flex::reflection_members_names_v<Address>) == "postalCode");
static_assert(std::get<3> (flex::reflection_members_names_v<Address>) == "city");
static_assert(std::get<4> (flex::reflection_members_names_v<Address>) == "country");
static_assert(std::is_same_v<flex::reflection_members_t<Address>, std::tuple<std::string, int, int, std::string, std::string>>);




TEST_CASE("reflection", "[reflection]") {
	std::println("Address<0> : '{}'", std::get<0> (flex::reflection_members_names_v<Address>));
	std::println("Address<1> : '{}'", std::get<1> (flex::reflection_members_names_v<Address>));
	std::println("Address<2> : '{}'", std::get<2> (flex::reflection_members_names_v<Address>));
	std::println("Address<3> : '{}'", std::get<3> (flex::reflection_members_names_v<Address>));
	std::println("Address<4> : '{}'", std::get<4> (flex::reflection_members_names_v<Address>));

	std::println("Person<0> : '{}'", std::get<0> (flex::reflection_members_names_v<Person>));
	std::println("Person<1> : '{}'", std::get<1> (flex::reflection_members_names_v<Person>));
	std::println("Person<2> : '{}'", std::get<2> (flex::reflection_members_names_v<Person>));
	std::println("Person<3> : '{}'", std::get<3> (flex::reflection_members_names_v<Person>));

	REQUIRE(1 == 1);
}
