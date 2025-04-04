#include <flex/memory/allocator.hpp>
#include <flex/memory/stackAllocator.hpp>
#include <flex/reflection/enums.hpp>
#include <flex/memory/poolAllocator.hpp>

#include <print>


auto main(int, char**) -> int {
	flex::error_type auto allocatorWithError {flex::memory::PoolAllocator<int>::make(32)};
	if (!allocatorWithError)
		return std::println(stderr, "Can't create pool"), EXIT_FAILURE;
	auto allocator {std::move(*allocatorWithError)};

	flex::error_type auto ptrWithError {flex::memory::allocator_traits<decltype(allocator)>::allocate(allocator, 1)};
	if (!ptrWithError)
		return std::println(stderr, "Can't allocate memory : {}",
			flex::toString(ptrWithError.error_or(flex::memory::AllocatorErrorCode::eFailure)).value_or("<invalid>")
		), EXIT_FAILURE;

	flex::memory::allocator_traits<decltype(allocator)>::construct(allocator, *ptrWithError, 0);

	std::println("{}", **ptrWithError);

	flex::memory::allocator_traits<decltype(allocator)>::deallocate(allocator, *ptrWithError, 1);
	return EXIT_SUCCESS;
}
