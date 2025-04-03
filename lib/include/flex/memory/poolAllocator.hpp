#pragma once

#include <atomic>
#include <cstddef>
#include <mutex>
#include <type_traits>

#include "flex/memory/allocator.hpp"
#include "flex/typeTraits.hpp"


namespace flex::memory {
	template <flex::memory::allocator Allocator>
	class PoolAllocatorView final {
		template <flex::memory::allocator, bool>
		friend class PoolAllocatorView;

		struct _SharedState;
		using _This = PoolAllocatorView<Allocator>;
		using _SharedStateAllocator = flex::memory::allocator_rebind_t<Allocator, _SharedState>;
		using _shared_state_allocator_traits = flex::memory::allocator_traits<_SharedStateAllocator>;
		using _SharedStatePointer = flex::memory::allocator_pointer_t<_SharedStateAllocator>;
		using _block_allocator_traits = flex::memory::allocator_traits<Allocator>;

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
			using RebindType = PoolAllocatorView<flex::memory::allocator_rebind_t<Allocator, Rebind>>;
			static constexpr bool IS_ALWAYS_EQUAL = false;

			PoolAllocatorView() noexcept = default;
			~PoolAllocatorView();

			PoolAllocatorView(const _This &allocator) noexcept;
			auto operator=(const _This &allocator) noexcept -> _This&;
			PoolAllocatorView(_This &&allocator) noexcept;
			auto operator=(_This &&allocator) noexcept -> _This&;

			inline auto operator==(const _This &allocator) const noexcept -> bool {
				return m_allocator == allocator.m_allocator && m_sharedState == allocator.m_sharedState;
			}

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

			auto copyOnContainerCopy() const noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode>;


		private:
			static auto s_make(
				SizeType size,
				_This &&allocator,
				_SharedStateAllocator &&sharedStateAllocator
			) noexcept -> std::expected<_This, flex::memory::AllocatorErrorCode>;

			struct _SharedState {
				PointerType pool;
				PointerType nextFree;
				int_fast32_t instanceCount;
				SizeType size;

				[[no_unique_address]]
				flex::enable_field_if_t<IS_ALLOCATOR_STORED, _SharedStateAllocator> sharedStateAllocator;
			};

			[[no_unique_address]]
			flex::enable_field_if_t<IS_ALLOCATOR_STORED, Allocator> m_allocator;
			_SharedStatePointer m_sharedState;
	};


	template <typename T>
	using PoolAllocator = PoolAllocatorView<flex::memory::Allocator<T>>;

} // namespace flex::memory

#include "flex/memory/poolAllocator.inl"
