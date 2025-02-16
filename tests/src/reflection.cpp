#include <print>

#include <flex/reflection/reflection.hpp>
#include <catch2/catch_test_macros.hpp>

struct Address {
	std::string street;
	int number;
	int postalCode;
	std::string city;

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
			&Address::postalCode,
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
static_assert(std::get<1> (flex::reflection_members_names_v<Address>) == "streetNumber");
static_assert(std::get<2> (flex::reflection_members_names_v<Address>) == "city");
static_assert(std::get<3> (flex::reflection_members_names_v<Address>) == "postalCode");
static_assert(std::get<4> (flex::reflection_members_names_v<Address>) == "country");
static_assert(std::is_same_v<flex::reflection_members_t<Address>, std::tuple<std::string, int, std::string, int, std::string>>);



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

	Person person {};
	flex::reflection_traits<Person>::getMember<0> (person) = "Albert";
	flex::reflection_traits<Person>::getMember<1> (person) = "Einstein";
	flex::reflection_traits<Person>::getMember<2> (person) = 76;
	flex::reflection_traits<Address>::getMember<0> (flex::reflection_traits<Person>::getMember<3> (person)) = "Kramgasse";
	flex::reflection_traits<Address>::getMember<1> (flex::reflection_traits<Person>::getMember<3> (person)) = 12;
	flex::reflection_traits<Address>::getMember<2> (flex::reflection_traits<Person>::getMember<3> (person)) = "Bern";
	flex::reflection_traits<Address>::getMember<3> (flex::reflection_traits<Person>::getMember<3> (person)) = 3000;
	flex::reflection_traits<Address>::getMember<4> (flex::reflection_traits<Person>::getMember<3> (person)) = "Switzerland";

	REQUIRE(person.name == "Albert");
	REQUIRE(person.surname == "Einstein");
	REQUIRE(person.age == 76);
	REQUIRE(person.address.street == "Kramgasse");
	REQUIRE(person.address.number == 12);
	REQUIRE(person.address.postalCode == 3000);
	REQUIRE(person.address.city == "Bern");
	REQUIRE(person.address.getCountry() == "Switzerland");
}
