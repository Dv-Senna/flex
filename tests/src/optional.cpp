#include <print>

#include <flex/pipes/optional.hpp>


int main() {
	using namespace std::string_literals;
	std::optional<float> opt {1036.97f};
	std::println("Money : {}$ !", opt
		| flex::pipes::transform([](float val) -> std::optional<float> {
			if (val < 0.f)
				return std::nullopt;
			return val * 1.01f;
		})
		| flex::pipes::to_string(std::nullopt, std::chars_format::fixed, 2)
		| flex::pipes::value_or("invalid amount")
	);

	return 0;
}
