#include <flex/memory/allocator.hpp>

#include <print>


auto main(int, char**) -> int {
	flex::memory::Allocator<int> allocator {};

	std::optional ptrWithError {flex::memory::allocator_traits<decltype(allocator)>::allocate(allocator, 16)};
	if (!ptrWithError)
		return std::println(stderr, "Can't allocate memory"), EXIT_FAILURE;

	for (int i {0}; i < 16; ++i)
		flex::memory::allocator_traits<decltype(allocator)>::construct(allocator, *ptrWithError + i, i);

	for (std::size_t i {0}; i < 16; ++i)
		std::println("{}", (*ptrWithError)[i]);

	flex::memory::allocator_traits<decltype(allocator)>::deallocate(allocator, *ptrWithError, 16);
	return EXIT_SUCCESS;
}
