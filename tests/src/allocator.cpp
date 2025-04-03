#include <flex/memory/allocator.hpp>
#include <flex/memory/stackAllocator.hpp>
#include <flex/reflection/enums.hpp>

#include <print>


auto main(int, char**) -> int {
	flex::error_type auto allocatorWithError {flex::memory::StackAllocator<int>::make(32)};
	if (!allocatorWithError)
		return std::println(stderr, "Can't create stack"), EXIT_FAILURE;
	auto allocator {std::move(*allocatorWithError)};

	flex::error_type auto ptrWithError {flex::memory::allocator_traits<decltype(allocator)>::allocate(allocator, 16)};
	if (!ptrWithError)
		return std::println(stderr, "Can't allocate memory : {}",
			flex::toString(ptrWithError.error_or(flex::memory::AllocatorErrorCode::eFailure)).value_or("<invalid>")
		), EXIT_FAILURE;

	for (int i {0}; i < 16; ++i)
		flex::memory::allocator_traits<decltype(allocator)>::construct(allocator, *ptrWithError + i, i);

	for (std::size_t i {0}; i < 16; ++i)
		std::println("{}", (*ptrWithError)[i]);

	flex::memory::allocator_traits<decltype(allocator)>::deallocate(allocator, *ptrWithError, 16);
	return EXIT_SUCCESS;
}
