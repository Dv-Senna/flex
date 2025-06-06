#include <limits>
#include <print>

#include <flex/reflection/enums.hpp>

#include <flex/pipes/andThen.hpp>
#include <flex/pipes/conversion.hpp>
#include <flex/pipes/hasValue.hpp>
#include <flex/pipes/transform.hpp>
#include <flex/pipes/toNumber.hpp>
#include <flex/pipes/toString.hpp>
#include <flex/pipes/valueOr.hpp>


class Foo {
	public:
		Foo(int) {}
};

enum class SomeEnum {
	eA,
	eB,
	eC
};

struct A {
	virtual auto hello() -> void {};
};
struct B : A {};
struct C {
	virtual auto hello() -> void {};
};

int main() {
	using namespace std::string_literals;
	std::optional<float> opt {-1036.97f};
	std::println("Money : {}$ !", opt
		| flex::pipes::and_then([](float val) -> std::optional<float> {
			if (val < 0.f)
				return std::nullopt;
			return val * 1.01f;
		})
		| flex::pipes::to_string()
		| flex::pipes::to_number<float> ()
		| flex::pipes::value_or(std::numeric_limits<float>::quiet_NaN())
//		| flex::pipes::value_or("invalid amount")
	);

	std::any any {2};
	std::println("Any as float : {}", any
		| flex::pipes::any_cast_to<float> ()
		| flex::pipes::value_or(std::numeric_limits<float>::quiet_NaN())
	);

	std::println("Any as int : {}", any
		| flex::pipes::any_cast_to<int> ()
		| flex::pipes::static_cast_to<float> ()
		| flex::pipes::value_or(std::numeric_limits<float>::quiet_NaN())
	);

	std::println("Any to foo : {}", any
		| flex::pipes::any_cast_to<int> ()
		| flex::pipes::construct_to<Foo> ()
		| flex::pipes::transform([](auto&&){return "FOO";})
		| flex::pipes::value_or("<INVALID>")
	);

	std::println("Value as string from enum : {}", SomeEnum::eA
		| flex::pipes::to_string()
		| flex::pipes::value_or("<INVALID>")
	);

	A a {};
	B b {};
	C c {};

	std::println("A->A : {}", a | flex::pipes::dynamic_cast_to<A&> () | flex::pipes::has_value());
	std::println("A->B : {}", a | flex::pipes::dynamic_cast_to<B&> () | flex::pipes::has_value());
	std::println("A->C : {}", a | flex::pipes::dynamic_cast_to<C&> () | flex::pipes::has_value());
	std::println("B->A : {}", b | flex::pipes::dynamic_cast_to<A&> () | flex::pipes::has_value());
	std::println("B->B : {}", b | flex::pipes::dynamic_cast_to<B&> () | flex::pipes::has_value());
	std::println("B->C : {}", b | flex::pipes::dynamic_cast_to<C&> () | flex::pipes::has_value());
	std::println("C->A : {}", c | flex::pipes::dynamic_cast_to<A&> () | flex::pipes::has_value());
	std::println("C->B : {}", c | flex::pipes::dynamic_cast_to<B&> () | flex::pipes::has_value());
	std::println("C->C : {}", c | flex::pipes::dynamic_cast_to<C&> () | flex::pipes::has_value());

	return 0;
}
