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


static_assert(flex::reflection::getAggregateMembersCount<Address> () == 5);
static_assert(flex::reflection::getAggregateMembersCount<Person> () == 4);


TEST_CASE("reflection", "[reflection]") {
	std::println("Person<0> : '{}'", flex::reflection::getAggregateMemberName<0, Person> ());

	REQUIRE(1 == 1);
}
