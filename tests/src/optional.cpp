#include <limits>
#include <print>

#include <flex/pipes/andThen.hpp>
#include <flex/pipes/optional.hpp>
#include <flex/pipes/transform.hpp>
#include <flex/pipes/toString.hpp>
#include <flex/pipes/valueOr.hpp>


int main() {
	using namespace std::string_literals;
	std::optional<float> opt {1036.97f};
	std::println("Money : {}$ !", opt
		| flex::pipes::and_then([](float val) -> std::optional<float> {
			if (val < 0.f)
				return std::nullopt;
			return val * 1.01f;
		})
		| flex::pipes::to_string()
/*		| flex::pipes::to_number<float> ()
		| flex::pipes::value_or(std::numeric_limits<float>::quiet_NaN())*/
		| flex::pipes::value_or("invalid amount")
	);

	return 0;
}
