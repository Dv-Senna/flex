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

#include <flex/reflection/reflection.hpp>


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


static_assert(flex::reflection::reflectable<Address>);
static_assert(flex::reflection::reflectable<Person>);


auto main() -> int {
	static_assert(flex::reflection::aggregate::member_count_v<Address> == 5);
	static_assert(flex::reflection::aggregate::member_count_v<Person> == 4);

	static_assert(std::same_as<
		flex::reflection::reflection_traits<Address>::member_types,
		std::tuple<std::string, int, int, std::string, std::string>
	>);

	Address address {};
	flex::reflection::reflection_traits<Address>::getMember<0u> (address) = "Kramgasse";
	flex::reflection::reflection_traits<Address>::getMember<1u> (address) = 49;
	flex::reflection::reflection_traits<Address>::getMember<2u> (address) = 3000;
	flex::reflection::reflection_traits<Address>::getMember<3u> (address) = "Bern";
	flex::reflection::reflection_traits<Address>::getMember<4u> (address) = "Switzerland";

/*	std::println("Address: {} {}, {} {}, {}",
		address.street, address.number,
		address.code, address.town,
		address.country
	);

	std::println("Name of Address's members:");
	for (const auto name : flex::reflection::aggregate::getMemberNames<Address> ())
		std::println("\t- '{}'", name);*/
	std::println("Address:");
	flex::reflection::foreachNamedMember(address, [](auto& member, std::string_view name) {
		std::println("\t- {:7} = {}", name, member);
	});

	static_assert(!noexcept(
		flex::reflection::foreachNamedMember(address, [](auto& member, std::string_view name) {})
	));
	static_assert(noexcept(
		flex::reflection::foreachNamedMember(address, [](auto& member, std::string_view name) noexcept {})
	));

	return EXIT_SUCCESS;
}
