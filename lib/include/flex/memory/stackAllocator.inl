#pragma once

#include "flex/memory/stackAllocator.hpp"

#include "flex/errorCode.hpp"


namespace flex::memory {
	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	StackAllocatorView<Allocator, THREAD_SAFE>::~StackAllocatorView() {
		if (m_sharedState == _SharedStatePointer{})
			return;
		--m_sharedState->instanceCount;
		if (m_sharedState->instanceCount != 0)
			return;

		if (m_sharedState->rbp != VoidPointerType{})
			(void)_unified_allocator_traits::deallocate(m_sharedState->unifiedAllocator, m_sharedState->rbp);
		_SharedStateAllocator sharedStateAllocator {std::move(m_sharedState->sharedStateAllocator)};
		(void)_shared_state_allocator_traits::deallocate(sharedStateAllocator, m_sharedState);
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	StackAllocatorView<Allocator, THREAD_SAFE>::StackAllocatorView(const _This &allocator) noexcept :
		m_allocator {allocator.m_allocator},
		m_sharedState {allocator.m_sharedState}
	{
		if (m_sharedState != _SharedStatePointer{})
			++m_sharedState->instanceCount;
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::operator=(const _This &allocator) noexcept -> _This& {
		this->~StackAllocatorView<Allocator, THREAD_SAFE> ();
		new (this) _This {allocator};
		return *this;
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	StackAllocatorView<Allocator, THREAD_SAFE>::StackAllocatorView(_This &&allocator) noexcept :
		m_allocator {std::move(allocator.m_allocator)},
		m_sharedState {std::move(allocator.m_sharedState)}
	{
		allocator.m_sharedState = _SharedStatePointer{};
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::operator=(_This &&allocator) noexcept -> _This& {
		this->~StackAllocatorView<Allocator, THREAD_SAFE> ();
		new (this) _This {std::move(allocator)};
		return *this;
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::make(
		SizeType size
	) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> requires (!IS_ALLOCATOR_STORED) {
		return s_make(size, _This{}, _SharedStateAllocator{}, _UnifiedAllocator{});
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	template <flex::memory::allocator Allocator2>
	requires std::same_as<std::remove_cvref_t<Allocator2>, Allocator>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::make(
		Allocator2 &&allocator, SizeType size
	) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> requires (IS_ALLOCATOR_STORED) {
		_This instance {};
		instance.m_allocator = std::forward<Allocator2> (allocator);

		flex::error_type auto sharedStateAllocatorWithError {flex::memory::allocator_traits<Allocator>::template rebind<_SharedState> (instance.m_allocator)};
		using shared_state_traits = flex::error_type_traits<decltype(sharedStateAllocatorWithError)>;
		if (!shared_state_traits::hasValue(sharedStateAllocatorWithError))
			return std::unexpected{flex::memory::AllocatorErrorCode::eStackSharedAllocatorRebindFailure};
		_SharedStateAllocator &&sharedStateAllocator {std::move(shared_state_traits::getValue(sharedStateAllocatorWithError))};

		flex::error_type auto unifiedAllocatorWithError {flex::memory::allocator_traits<Allocator>::template rebind<std::byte> (instance.m_allocator)};
		using unified_state_traits = flex::error_type_traits<decltype(unifiedAllocatorWithError)>;
		if (!unified_state_traits::hasValue(unifiedAllocatorWithError))
			return std::unexpected{flex::memory::AllocatorErrorCode::eStackUnifiedAllocatorRebindFailure};
		_UnifiedAllocator &&unifiedAllocator {std::move(unified_state_traits::getValue(unifiedAllocatorWithError))};

		return s_make(size, std::move(instance), sharedStateAllocator, unifiedAllocator);
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::allocate(std::size_t n) noexcept -> std::expected<PointerType, flex::memory::AllocatorErrorCode> {
		if (m_sharedState == _SharedStatePointer{} || m_sharedState->rbp == VoidPointerType{})
			return std::unexpected{flex::memory::AllocatorErrorCode::eStackNotInitialized};

		auto address {std::to_address(m_sharedState->rsp)};
		auto alignmentOffset {static_cast<std::uint64_t> (address) % alignof(ValueType)};
		if (alignmentOffset != 0)
			alignmentOffset = alignof(ValueType) - alignmentOffset;

		SizeType size {n * sizeof(ValueType)};
		DifferenceType offset {m_sharedState->rsp - m_sharedState->rbp};
		if (offset + alignmentOffset + size > m_sharedState->size)
			return std::unexpected{flex::memory::AllocatorErrorCode::eStackTooSmall};

		m_sharedState->rsp += alignmentOffset;
		auto ptr {m_sharedState->rsp};
		m_sharedState->rsp += size;
		return ptr;
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::deallocate(const PointerType &ptr, std::size_t) noexcept -> flex::memory::AllocatorErrorCode {
		if (m_sharedState == _SharedStatePointer{} || m_sharedState->rbp == VoidPointerType{})
			return flex::memory::AllocatorErrorCode::eStackNotInitialized;
		if (ptr < m_sharedState->rbp || ptr >= m_sharedState->rsp)
			return flex::memory::AllocatorErrorCode::eStackInvalidPtr;
		m_sharedState->rsp = ptr;
		return flex::memory::AllocatorErrorCode::eSuccess;
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	template <typename ...Args>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::construct(const PointerType &ptr, Args &&...args) noexcept {
		if constexpr (IS_ALLOCATOR_STORED)
			return flex::memory::allocator_traits<Allocator>::construct(m_allocator, ptr, std::forward<Args> (args)...);
		else
			return flex::memory::allocator_traits<Allocator>::construct(Allocator{}, ptr, std::forward<Args> (args)...);
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::destroy(const PointerType &ptr) noexcept {
		if constexpr (IS_ALLOCATOR_STORED)
			return flex::memory::allocator_traits<Allocator>::destroy(m_allocator, ptr);
		else
			return flex::memory::allocator_traits<Allocator>::destroy(Allocator{}, ptr);
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	template <typename Rebind>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::rebind() const noexcept -> std::optional<RebindType<Rebind>> {
		RebindType<Rebind> rebind {};
		if constexpr (IS_ALLOCATOR_STORED)
			rebind.m_allocator = flex::memory::allocator_traits<Allocator>::template rebind<Rebind> (m_allocator);

		rebind.m_sharedState = m_sharedState;
		if (rebind.m_sharedState != _SharedStatePointer{})
			++rebind.m_sharedState->instanceCount;
		return rebind;
	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::copyOnContainerCopy() const noexcept -> std::optional<_This> {

	}


	template <flex::memory::allocator Allocator, bool THREAD_SAFE>
	auto StackAllocatorView<Allocator, THREAD_SAFE>::s_make(
		SizeType size,
		_This &&allocator,
		_SharedStateAllocator &&sharedStateAllocator,
		_UnifiedAllocator &&unifiedAllocator
	) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> {
		flex::error_type auto sharedStateWithError {_shared_state_allocator_traits::allocate(sharedStateAllocator, 1)};
		using shared_state_traits = flex::error_type_traits<decltype(sharedStateWithError)>;
		if (!shared_state_traits::hasValue(sharedStateWithError))
			return std::unexpected{flex::memory::AllocatorErrorCode::eStackSharedStateAllocationFailure};
		allocator.m_sharedState = std::move(shared_state_traits::getValue(sharedStateWithError));

		flex::error_code auto sharedStateConstructResult {_shared_state_allocator_traits::construct(sharedStateAllocator, allocator.m_sharedState)};
		if (!flex::error_type_traits<decltype(sharedStateConstructResult)>::isSuccess(sharedStateConstructResult))
			return std::unexpected{flex::memory::AllocatorErrorCode::eStackSharedStateCreationFailure};

		++size;
		flex::error_type auto blockWithError {_unified_allocator_traits::allocate(unifiedAllocator, sizeof(ValueType) * size)};
		using block_state_traits = flex::error_type_traits<decltype(blockWithError)>;
		if (!block_state_traits::hasValue(blockWithError))
			return std::unexpected{flex::memory::AllocatorErrorCode::eStackBlockAllocationFailure};

		allocator.m_sharedState->rbp = static_cast<VoidPointerType> (std::move(block_state_traits::getValue(blockWithError)));
		allocator.m_sharedState->rsp = allocator.m_sharedState->rbp;
		allocator.m_sharedState->instanceCount = 1;
		allocator.m_sharedState->size = size;

		if constexpr (IS_ALLOCATOR_STORED) {
			allocator.m_sharedState->sharedStateAllocator = std::move(sharedStateAllocator);
			allocator.m_sharedState->unifiedAllocator = std::move(unifiedAllocator);
		}

		return allocator;
	}


} // flex::memory
