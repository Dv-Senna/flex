#include <iostream>

#include <flex/logger.hpp>
#include <flex/reflection/reflection.hpp>
#include <catch2/catch_test_macros.hpp>

struct City {
	int postalCode;
	std::string city;
};

struct Address {
	std::string street;
	int number;
	City city;

	constexpr auto getCountry() const noexcept -> const std::string& {return m_country;}
	constexpr auto setCountry(const std::string &value) noexcept -> void {m_country = value;}

private:
	std::string m_country;

public:
	struct FlexMetadata {
		static constexpr std::tuple MEMBERS {
			&Address::street,
			std::tuple{"streetNumber", &Address::number},
			&Address::city,
			std::tuple{"country", &Address::getCountry, &Address::setCountry}
		};
	};
};

struct Person {
	std::string name;
	std::string surname;
	int age;
	Address address;
	auto test() -> void {}
};


static_assert(!flex::reflectable<decltype("{")>);

static_assert(flex::is_reflectable_v<Person>);
static_assert(flex::reflection_members_count_v<Person> == 4);
static_assert(std::get<0> (flex::reflection_members_names_v<Person>) == "name");
static_assert(std::get<1> (flex::reflection_members_names_v<Person>) == "surname");
static_assert(std::get<2> (flex::reflection_members_names_v<Person>) == "age");
static_assert(std::get<3> (flex::reflection_members_names_v<Person>) == "address");
static_assert(std::is_same_v<flex::reflection_members_t<Person>, std::tuple<std::string, std::string, int, Address>>);

static_assert(flex::is_reflectable_v<Address>);
static_assert(flex::reflection_members_count_v<Address> == 4);
static_assert(std::get<0> (flex::reflection_members_names_v<Address>) == "street");
static_assert(std::get<1> (flex::reflection_members_names_v<Address>) == "streetNumber");
static_assert(std::get<2> (flex::reflection_members_names_v<Address>) == "city");
static_assert(std::get<3> (flex::reflection_members_names_v<Address>) == "country");
static_assert(std::is_same_v<flex::reflection_members_t<Address>, std::tuple<std::string, int, City, std::string>>);



TEST_CASE("reflection", "[reflection]") {
	flex::Logger::info("Address<0> : '{}'", std::get<0> (flex::reflection_members_names_v<Address>));
	flex::Logger::info("Address<1> : '{}'", std::get<1> (flex::reflection_members_names_v<Address>));
	flex::Logger::info("Address<2> : '{}'", std::get<2> (flex::reflection_members_names_v<Address>));
	flex::Logger::info("Address<3> : '{}'", std::get<3> (flex::reflection_members_names_v<Address>));

	flex::Logger::info("Person<0> : '{}'", std::get<0> (flex::reflection_members_names_v<Person>));
	flex::Logger::info("Person<1> : '{}'", std::get<1> (flex::reflection_members_names_v<Person>));
	flex::Logger::info("Person<2> : '{}'", std::get<2> (flex::reflection_members_names_v<Person>));
	flex::Logger::info("Person<3> : '{}'", std::get<3> (flex::reflection_members_names_v<Person>));


	Person person {};
	flex::reflection_traits<Person>::getMember<0> (person) = "Albert";
	flex::reflection_traits<Person>::getMember<1> (person) = "Einstein";
	flex::reflection_traits<Person>::getMember<2> (person) = 76;
	flex::reflection_traits<Address>::getMember<0> (flex::reflection_traits<Person>::getMember<3> (person)) = "Kramgasse";
	flex::reflection_traits<Address>::getMember<1> (flex::reflection_traits<Person>::getMember<3> (person)) = 12;
	flex::reflection_traits<Address>::getMember<2> (flex::reflection_traits<Person>::getMember<3> (person)).city = "Bern";
	flex::reflection_traits<Address>::getMember<2> (flex::reflection_traits<Person>::getMember<3> (person)).postalCode = 3000;
	flex::reflection_traits<Address>::getMember<3> (flex::reflection_traits<Person>::getMember<3> (person)) = "Switzerland";

	flex::Logger::info("person : {}", person);
	flex::Logger::info("person styled : {:4}", person);

	flex::Logger::setMinLevel(flex::LogLevel::eVerbose);
	flex::Logger::verbose("Some log");
	flex::Logger::info("Some log");
	flex::Logger::warning("Some log");
	flex::Logger::error("Some log");
	flex::Logger::fatal("Some log");

	REQUIRE(person.name == "Albert");
	REQUIRE(person.surname == "Einstein");
	REQUIRE(person.age == 76);
	REQUIRE(person.address.street == "Kramgasse");
	REQUIRE(person.address.number == 12);
	REQUIRE(person.address.city.postalCode == 3000);
	REQUIRE(person.address.city.city == "Bern");
	REQUIRE(person.address.getCountry() == "Switzerland");
}
