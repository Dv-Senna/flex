#include <limits>
#include <print>

#include <flex/pipes/andThen.hpp>
#include <flex/pipes/conversion.hpp>
#include <flex/pipes/transform.hpp>
#include <flex/pipes/toNumber.hpp>
#include <flex/pipes/toString.hpp>
#include <flex/pipes/valueOr.hpp>


class Foo {
	public:
		Foo(int) {}
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
		| flex::pipes::value_or("INVALID")
	);

	return 0;
}
