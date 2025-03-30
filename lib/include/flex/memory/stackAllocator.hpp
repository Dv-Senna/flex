#pragma once

#include <atomic>
#include <cstddef>
#include <mutex>
#include <type_traits>

#include "flex/errorType.hpp"
#include "flex/memory/allocator.hpp"
#include "flex/typeTraits.hpp"


namespace flex::memory {
	template <flex::memory::allocator Allocator, bool THREAD_SAFE = false>
	class StackAllocatorView final {
		template <flex::memory::allocator, bool>
		friend class StackAllocatorView;

		struct _SharedState;
		using _This = StackAllocatorView<Allocator, THREAD_SAFE>;
		using _UnifiedAllocator = flex::memory::allocator_rebind_t<Allocator, std::byte>;
		using _SharedStateAllocator = flex::memory::allocator_rebind_t<Allocator, _SharedState>;
		using _unified_allocator_traits = flex::memory::allocator_traits<_UnifiedAllocator>;
		using _shared_state_allocator_traits = flex::memory::allocator_traits<_SharedStateAllocator>;
		using _SharedStatePointer = flex::memory::allocator_pointer_t<_SharedStateAllocator>;

		static constexpr bool IS_ALLOCATOR_STORED = flex::memory::stateful_allocator<Allocator>;

		public:
			using ValueType = flex::memory::allocator_value_t<Allocator>;
			using PointerType = flex::memory::allocator_pointer_t<Allocator>;
			using ConstPointerType = flex::memory::allocator_const_pointer_t<Allocator>;
			using VoidPointerType = flex::memory::allocator_void_pointer_t<Allocator>;
			using ConstVoidPointerType = flex::memory::allocator_const_void_pointer_t<Allocator>;
			using SizeType = flex::memory::allocator_size_t<Allocator>;
			using DifferenceType = flex::memory::allocator_difference_t<Allocator>;
			template <typename Rebind>
			using RebindType = StackAllocatorView<flex::memory::allocator_rebind_t<Allocator, Rebind>, THREAD_SAFE>;
			static constexpr bool IS_ALWAYS_EQUAL = false;

			StackAllocatorView() noexcept = default;
			~StackAllocatorView();

			StackAllocatorView(const _This &allocator) noexcept;
			auto operator=(const _This &allocator) noexcept -> _This&;
			StackAllocatorView(_This &&allocator) noexcept;
			auto operator=(_This &&allocator) noexcept -> _This&;

			static auto make(SizeType size) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> requires (!IS_ALLOCATOR_STORED);
			template <flex::memory::allocator Allocator2>
			requires std::same_as<std::remove_cvref_t<Allocator2>, Allocator>
			static auto make(Allocator2 &&allocator, SizeType size) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode> requires (IS_ALLOCATOR_STORED);

			auto allocate(std::size_t n) noexcept -> std::expected<PointerType, flex::memory::AllocatorErrorCode>;
			auto deallocate(const PointerType &ptr, std::size_t n) noexcept -> flex::memory::AllocatorErrorCode;

			template <typename ...Args>
			auto construct(const PointerType &ptr, Args &&...args) noexcept;
			auto destroy(const PointerType &ptr) noexcept;

			template <typename Rebind>
			auto rebind() const noexcept -> std::optional<RebindType<Rebind>>;

			auto copyOnContainerCopy() const noexcept -> std::optional<_This>;


		private:
			static auto s_make(
				SizeType size,
				_This &&allocator,
				_SharedStateAllocator &&sharedStateAllocator,
				_UnifiedAllocator &&unifiedAllocator
			) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode>;

			struct _SharedState {
				[[no_unique_address]]
				flex::enable_field_if_t<THREAD_SAFE, std::mutex> mutex;

				VoidPointerType rbp;
				VoidPointerType rsp;
				std::conditional_t<THREAD_SAFE, std::atomic_int_fast32_t, int_fast32_t> instanceCount;
				SizeType size;

				[[no_unique_address]]
				flex::enable_field_if_t<IS_ALLOCATOR_STORED, _SharedStateAllocator> sharedStateAllocator;
				[[no_unique_address]]
				flex::enable_field_if_t<IS_ALLOCATOR_STORED, _UnifiedAllocator> unifiedAllocator;
			};

			[[no_unique_address]]
			flex::enable_field_if_t<IS_ALLOCATOR_STORED, Allocator> m_allocator;
			_SharedStatePointer m_sharedState;
	};

} // namespace flex::memory

#include "flex/memory/stackAllocator.inl"
