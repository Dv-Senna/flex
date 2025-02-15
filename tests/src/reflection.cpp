#include <print>

#include <flex/reflection/aggregate.hpp>
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

static_assert(flex::reflection::getAggregateMembersCount<Person> () == 4);
static_assert(flex::reflection::getAggregateMemberName<0, Person> () == "name");
static_assert(flex::reflection::getAggregateMemberName<1, Person> () == "surname");
static_assert(flex::reflection::getAggregateMemberName<2, Person> () == "age");
static_assert(flex::reflection::getAggregateMemberName<3, Person> () == "address");

static_assert(flex::reflection::getAggregateMembersCount<Address> () == 5);
static_assert(flex::reflection::getAggregateMemberName<0, Address> () == "street");
static_assert(flex::reflection::getAggregateMemberName<1, Address> () == "number");
static_assert(flex::reflection::getAggregateMemberName<2, Address> () == "postalCode");
static_assert(flex::reflection::getAggregateMemberName<3, Address> () == "city");
static_assert(flex::reflection::getAggregateMemberName<4, Address> () == "country");




TEST_CASE("reflection", "[reflection]") {
	std::println("Address<0> : '{}'", flex::reflection::getAggregateMemberName<0, Address> ());
	std::println("Address<1> : '{}'", flex::reflection::getAggregateMemberName<1, Address> ());
	std::println("Address<2> : '{}'", flex::reflection::getAggregateMemberName<2, Address> ());
	std::println("Address<3> : '{}'", flex::reflection::getAggregateMemberName<3, Address> ());
	std::println("Address<4> : '{}'", flex::reflection::getAggregateMemberName<4, Address> ());

	std::println("Person<0> : '{}'", flex::reflection::getAggregateMemberName<0, Person> ());
	std::println("Person<1> : '{}'", flex::reflection::getAggregateMemberName<1, Person> ());
	std::println("Person<2> : '{}'", flex::reflection::getAggregateMemberName<2, Person> ());
	std::println("Person<3> : '{}'", flex::reflection::getAggregateMemberName<3, Person> ());

	REQUIRE(1 == 1);
}
