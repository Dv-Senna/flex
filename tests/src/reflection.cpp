#include <cstdlib>
#include <string>
#include <version>

#ifdef __cpp_lib_print
	#include <print>
#else
	#include <iostream>
	#include <format>

	namespace std {
		template <typename ...Args>
		inline auto print(std::format_string<Args...> format, Args&&... args) -> void {
			std::cout << std::format(format, std::forward<Args> (args)...);
		}

		template <typename ...Args>
		inline auto println(std::format_string<Args...> format, Args&&... args) -> void {
			std::cout << std::format(format, std::forward<Args> (args)...) << std::endl;
		}
	}
#endif

#include <flex/reflection/aggregate.hpp>

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


auto main() -> int {
	static_assert(flex::reflection::aggregate::member_count_v<Address> == 5);
	static_assert(flex::reflection::aggregate::member_count_v<Person> == 4);

	Address address {};
	std::get<0> (flex::reflection::aggregate::getMemberTie(address)) = "Kramgasse";
	std::get<1> (flex::reflection::aggregate::getMemberTie(address)) = 49;
	std::get<2> (flex::reflection::aggregate::getMemberTie(address)) = 3000;
	std::get<3> (flex::reflection::aggregate::getMemberTie(address)) = "Bern";
	std::get<4> (flex::reflection::aggregate::getMemberTie(address)) = "Switzerland";

	std::println("Address: {} {}, {} {}, {}",
		address.street, address.number,
		address.code, address.town,
		address.country
	);

	return EXIT_SUCCESS;
}
