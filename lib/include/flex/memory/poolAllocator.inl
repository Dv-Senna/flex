#pragma once

#include "flex/memory/poolAllocator.hpp"


namespace flex::memory {
	template <flex::memory::allocator Allocator>
	PoolAllocatorView<Allocator>::~PoolAllocatorView() {
		if (m_sharedState == _SharedStatePointer{})
			return;
		--m_sharedState->instanceCount;
		if (m_sharedState->instanceCount != 0)
			return;
	}


	template <flex::memory::allocator Allocator>
	PoolAllocatorView<Allocator>::PoolAllocatorView(const _This &allocator) noexcept :
		m_allocator {allocator.m_allocator},
		m_sharedState {allocator.m_sharedState}
	{
		if (m_sharedState != _SharedStatePointer{})
			++m_sharedState->instanceCount;
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::operator=(const _This &allocator) noexcept -> _This& {
		this->~PoolAllocatorView<Allocator> ();
		new (this) PoolAllocatorView<Allocator> (allocator);
		return *this;
	}


	template <flex::memory::allocator Allocator>
	PoolAllocatorView<Allocator>::PoolAllocatorView(_This &&allocator) noexcept :
		m_allocator {std::move(allocator.m_allocator)},
		m_sharedState {std::move(allocator.m_sharedState)}
	{
		allocator.m_sharedState = _SharedStatePointer{};
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::operator=(_This &&allocator) noexcept -> _This& {
		this->~PoolAllocatorView<Allocator> ();
		new (this) PoolAllocatorView<Allocator> (std::move(allocator));
		return *this;
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::make(SizeType size) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> requires (!IS_ALLOCATOR_STORED) {
		_This instance {};
		return _This::s_make(size, std::move(instance), _SharedStateAllocator{});
	}


	template <flex::memory::allocator Allocator>
	template <flex::memory::allocator Allocator2>
	requires std::same_as<std::remove_cvref_t<Allocator2>, Allocator>
	auto PoolAllocatorView<Allocator>::make(
		Allocator2 &&allocator, SizeType size
	) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> requires (IS_ALLOCATOR_STORED) {
		_This instance {};
		instance.m_allocator = std::forward<Allocator2> (allocator);

		flex::error_type auto sharedStateAllocatorWithError {flex::memory::allocator_traits<Allocator>::template rebind<_SharedState> (instance.m_allocator)};
		using shared_state_traits = flex::error_type_traits<decltype(sharedStateAllocatorWithError)>;
		if (!shared_state_traits::hasValue(sharedStateAllocatorWithError))
			return std::unexpected{flex::memory::AllocatorErrorCode::ePoolSharedAllocatorRebindFailure};
		_SharedStateAllocator &&sharedStateAllocator {std::move(shared_state_traits::getValue(sharedStateAllocatorWithError))};

		return _This::s_make(size, std::move(instance), std::move(sharedStateAllocator));
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::allocate(std::size_t n) noexcept -> std::expected<PointerType, flex::memory::AllocatorErrorCode> {
		if (n != 1)
			return std::unexpected(flex::memory::AllocatorErrorCode::ePoolAllocationSizeNotOne);
		if (m_sharedState == _SharedStatePointer{})
			return std::unexpected(flex::memory::AllocatorErrorCode::ePoolNotInitialized);
		if (m_sharedState->nextFree == PointerType{})
			return std::unexpected(flex::memory::AllocatorErrorCode::ePoolIsFull);

		PointerType ptr {std::move(m_sharedState->nextFree)};
		m_sharedState->nextFree = std::move(reinterpret_cast<PointerType> (*ptr));
		return ptr;
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::deallocate(const PointerType &ptr, std::size_t n) noexcept -> flex::memory::AllocatorErrorCode {
		if (n != 1)
			return flex::memory::AllocatorErrorCode::ePoolAllocationSizeNotOne;
		if (m_sharedState == _SharedStatePointer{})
			return flex::memory::AllocatorErrorCode::ePoolNotInitialized;
		if (ptr < m_sharedState->pool || ptr >= m_sharedState->pool + m_sharedState->size)
			return flex::memory::AllocatorErrorCode::ePoolInvalidPtr;

		reinterpret_cast<PointerType&> (*ptr) = std::move(m_sharedState->nextFree);
		m_sharedState->nextFree = ptr;
		return flex::memory::AllocatorErrorCode::eSuccess;
	}


	template <flex::memory::allocator Allocator>
	template <typename ...Args>
	auto PoolAllocatorView<Allocator>::construct(const PointerType &ptr, Args &&...args) noexcept {
		if constexpr (IS_ALLOCATOR_STORED)
			return _block_allocator_traits::construct(m_allocator, ptr, std::forward<Args> (args)...);
		else {
			Allocator allocator {};
			return _block_allocator_traits::construct(allocator, ptr, std::forward<Args> (args)...);
		}
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::destroy(const PointerType &ptr) noexcept {
		if constexpr (IS_ALLOCATOR_STORED)
			return _block_allocator_traits::destroy(m_allocator, ptr);
		else {
			Allocator allocator {};
			return _block_allocator_traits::destroy(allocator, ptr);
		}
	}


	template <flex::memory::allocator Allocator>
	template <typename Rebind>
	auto PoolAllocatorView<Allocator>::rebind() const noexcept -> std::expected<RebindType<Rebind>, flex::memory::AllocatorErrorCode> {
		if constexpr (IS_ALLOCATOR_STORED) {
			flex::error_type auto allocatorRebindWithError {_block_allocator_traits::template rebind<Rebind> (m_allocator)};
			using allocator_rebind_traits = flex::error_type_traits<decltype(allocatorRebindWithError)>;
			if (allocator_rebind_traits::hasValue(allocatorRebindWithError))
				return std::unexpected(flex::memory::AllocatorErrorCode::ePoolBlockAllocatorRebindFailure);
			flex::memory::allocator auto allocatorRebind {std::move(allocator_rebind_traits::getValue(allocatorRebindWithError))};

			if (m_sharedState == _SharedStatePointer{}) {
				RebindType<Rebind> rebindInstance {};
				rebindInstance.m_allocator = std::move(allocatorRebind);
				return rebindInstance;
			}
			return RebindType<Rebind>::make(std::move(allocatorRebind), m_sharedState->size);
		}
		else {
			if (m_sharedState == _SharedStatePointer{})
				return RebindType<Rebind> {};
			return RebindType<Rebind>::make(m_sharedState->size);
		}
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::copyOnContainerCopy() const noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> {
		if (m_sharedState == _SharedStatePointer{} || m_sharedState->size == 0)
			return _This{};
		if constexpr (IS_ALLOCATOR_STORED)
			return _This::make(m_allocator, m_sharedState->size);
		else
			return _This::make(m_sharedState->size);
	}


	template <flex::memory::allocator Allocator>
	auto PoolAllocatorView<Allocator>::s_make(
		SizeType size,
		_This &&allocator,
		_SharedStateAllocator &&sharedStateAllocator
	) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> {
		flex::error_type auto sharedStateWithError {_shared_state_allocator_traits::allocate(sharedStateAllocator, 1)};
		using shared_state_traits = flex::error_type_traits<decltype(sharedStateWithError)>;
		if (!shared_state_traits::hasValue(sharedStateWithError))
			return std::unexpected{flex::memory::AllocatorErrorCode::ePoolSharedStateAllocationFailure};
		allocator.m_sharedState = std::move(shared_state_traits::getValue(sharedStateWithError));

		if constexpr (flex::memory::is_allocator_construct_failable_v<_SharedStateAllocator>) {
			flex::error_code auto sharedStateConstructResult {_shared_state_allocator_traits::construct(sharedStateAllocator, allocator.m_sharedState)};
			if (!flex::error_type_traits<decltype(sharedStateConstructResult)>::isSuccess(sharedStateConstructResult))
				return std::unexpected{flex::memory::AllocatorErrorCode::ePoolSharedStateCreationFailure};
		}
		else
			_shared_state_allocator_traits::construct(sharedStateAllocator, allocator.m_sharedState);

		Allocator blockAllocator {};
		if constexpr (IS_ALLOCATOR_STORED)
			blockAllocator = allocator.m_allocator;

		flex::error_type auto blockWithError {_block_allocator_traits::allocate(blockAllocator, size)};
		using block_state_traits = flex::error_type_traits<decltype(blockWithError)>;
		if (!block_state_traits::hasValue(blockWithError))
			return std::unexpected{flex::memory::AllocatorErrorCode::ePoolBlockAllocationFailure};

		allocator.m_sharedState->pool = std::move(block_state_traits::getValue(blockWithError));
		allocator.m_sharedState->nextFree = allocator.m_sharedState->pool;
		allocator.m_sharedState->instanceCount = 1;
		allocator.m_sharedState->size = size;

		for (SizeType i {0}; i < size - 1; ++i)
			reinterpret_cast<PointerType&> (allocator.m_sharedState->pool[i]) = allocator.m_sharedState->pool + i + 1;
		reinterpret_cast<PointerType&> (allocator.m_sharedState->pool[size - 1]) = PointerType{};

		if constexpr (IS_ALLOCATOR_STORED)
			allocator.m_sharedState->sharedStateAllocator = std::move(sharedStateAllocator);
		return allocator;
	}

} // namespace flex::memory
