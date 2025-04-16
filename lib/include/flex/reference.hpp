#pragma once

#include "flex/operatorTraits.hpp"
#include "flex/typeTraits.hpp"


namespace flex {
	template <value_or_pointer T>
	class Reference final {
		using ReferenceType = std::add_lvalue_reference_t<T>;
		using ConstReferenceType = std::add_lvalue_reference_t<std::add_const_t<T>>;
		using PointerType = std::add_pointer_t<T>;

		public:
			Reference() = delete;
			constexpr ~Reference() = default;

			constexpr Reference(ReferenceType ref) noexcept : m_ref {std::addressof(ref)} {}

			constexpr Reference(const Reference<T>&) noexcept = default;
			constexpr auto operator=(const Reference<T>&) noexcept -> Reference<T>& = default;
			constexpr Reference(Reference<T>&&) noexcept = default;
			constexpr auto operator=(Reference<T>&&) noexcept -> Reference<T>& = default;

			constexpr auto get() const noexcept -> ReferenceType {return *m_ref;}
			constexpr operator ReferenceType() const noexcept {return *m_ref;}

			constexpr auto operator*() const noexcept -> ReferenceType requires (!flex::operators::dereferenceable<T>) {return *m_ref;}
			constexpr auto operator&() const noexcept -> PointerType requires (!flex::operators::addressable<T>) {return m_ref;}
			constexpr auto operator->() const noexcept -> PointerType requires (!flex::operators::member_dereferenceable<T>) {return m_ref;}

			constexpr auto operator*() const noexcept requires flex::operators::dereferenceable<T> {return **m_ref;}
			constexpr auto operator&() const noexcept requires flex::operators::addressable<T> {return &*m_ref;}
			constexpr auto operator->() const noexcept requires flex::operators::member_dereferenceable<T> {return m_ref->operator->();}

			template <typename ...Args>
			requires flex::operators::subscript_accessible<T, Args...>
			constexpr auto operator[](Args&& ...args) const noexcept {return (*m_ref)[std::forward<Args> (args)...];}

			constexpr auto operator==(ConstReferenceType val) const noexcept requires flex::operators::equality_comparable<T> {return *m_ref == val;}
			constexpr auto operator<=> (ConstReferenceType val) const noexcept requires flex::operators::three_way_comparable<T> {return *m_ref <=> val;}


		private:
			PointerType m_ref;
	};

} // namespace flex
