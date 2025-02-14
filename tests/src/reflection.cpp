#include <flex/reflection/autoAggregateSize.hpp>

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


static_assert(flex::reflection::getAggregateSize<Address> () == 5);
static_assert(flex::reflection::getAggregateSize<Person> () == 4);


TEST_CASE("reflection", "[reflection]") {
	REQUIRE(1 == 1);
}
