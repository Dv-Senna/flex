#pragma once

#include "flex/core/comptime.hpp"
#include "flex/reflection/reflection.hpp"
#include "flex/core/typeTraits.hpp"
#include <concepts>
#include <tuple>
#include <type_traits>


namespace flex::reflection {
	template <typename T>
	struct Comparator;

	template <typename T>
	concept valid_comparator = requires {
		{::flex::reflection::Comparator<T>::is_equal_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_noexcept_equal_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_three_way_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_noexcept_three_way_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_less_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_noexcept_less_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_less_or_equal_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_noexcept_less_or_equal_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_greater_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_noexcept_greater_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_greater_or_equal_comparable} -> std::same_as<const bool&>;
		{::flex::reflection::Comparator<T>::is_noexcept_greater_or_equal_comparable} -> std::same_as<const bool&>;
	};


	constexpr auto equal(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
		noexcept (Comparator<std::remove_cvref_t<decltype(lhs)>>::is_noexcept_equal_comparable)
		-> bool
		requires (valid_comparator<std::remove_cvref_t<decltype(lhs)>>
			&& Comparator<std::remove_cvref_t<decltype(lhs)>>::is_equal_comparable
		)
	{
		return Comparator<std::remove_cvref_t<decltype(lhs)>>::equal(lhs, rhs);
	}

	constexpr auto three_way(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
		noexcept (Comparator<std::remove_cvref_t<decltype(lhs)>>::is_noexcept_three_way_comparable)
		-> bool
		requires (valid_comparator<std::remove_cvref_t<decltype(lhs)>>
			&& Comparator<std::remove_cvref_t<decltype(lhs)>>::is_three_way_comparable
		)
	{
		return Comparator<std::remove_cvref_t<decltype(lhs)>>::three_way(lhs, rhs);
	}

	constexpr auto less(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
		noexcept (Comparator<std::remove_cvref_t<decltype(lhs)>>::is_noexcept_less_comparable)
		-> bool
		requires (valid_comparator<std::remove_cvref_t<decltype(lhs)>>
			&& Comparator<std::remove_cvref_t<decltype(lhs)>>::is_less_comparable
		)
	{
		return Comparator<std::remove_cvref_t<decltype(lhs)>>::less(lhs, rhs);
	}

	constexpr auto less_or_equal(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
		noexcept (Comparator<std::remove_cvref_t<decltype(lhs)>>::is_noexcept_less_or_equal_comparable)
		-> bool
		requires (valid_comparator<std::remove_cvref_t<decltype(lhs)>>
			&& Comparator<std::remove_cvref_t<decltype(lhs)>>::is_less_or_equal_comparable
		)
	{
		return Comparator<std::remove_cvref_t<decltype(lhs)>>::less_or_equal(lhs, rhs);
	}

	constexpr auto greater(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
		noexcept (Comparator<std::remove_cvref_t<decltype(lhs)>>::is_noexcept_greater_comparable)
		-> bool
		requires (valid_comparator<std::remove_cvref_t<decltype(lhs)>>
			&& Comparator<std::remove_cvref_t<decltype(lhs)>>::is_greater_comparable
		)
	{
		return Comparator<std::remove_cvref_t<decltype(lhs)>>::greater(lhs, rhs);
	}

	constexpr auto greater_or_equal(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
		noexcept (Comparator<std::remove_cvref_t<decltype(lhs)>>::is_noexcept_greater_or_equal_comparable)
		-> bool
		requires (valid_comparator<std::remove_cvref_t<decltype(lhs)>>
			&& Comparator<std::remove_cvref_t<decltype(lhs)>>::is_greater_or_equal_comparable
		)
	{
		return Comparator<std::remove_cvref_t<decltype(lhs)>>::greater_or_equal(lhs, rhs);
	}


	namespace internals {
		enum class CompOp {
			eEqual,
			eThreeWay,
			eLess,
			eLessOrEqual,
			eGreater,
			eGreaterOrEqual
		};


		template <typename T, CompOp op>
		concept op_comparable = []() consteval {
			if constexpr (op == CompOp::eEqual)
				return requires(const T val) {{val == val} -> std::same_as<bool>;};
			else if constexpr (op == CompOp::eThreeWay)
				return requires(const T val) {val <=> val;};
			else if constexpr (op == CompOp::eLess)
				return requires(const T val) {{val < val} -> std::same_as<bool>;};
			else if constexpr (op == CompOp::eLessOrEqual)
				return requires(const T val) {{val <= val} -> std::same_as<bool>;};
			else if constexpr (op == CompOp::eGreater)
				return requires(const T val) {{val > val} -> std::same_as<bool>;};
			else if constexpr (op == CompOp::eGreaterOrEqual)
				return requires(const T val) {{val >= val} -> std::same_as<bool>;};
			else
				flex::comptimeError("Invalid operation");
			return false;
		} ();

		template <typename T, CompOp op>
		concept noexcept_op_comparable = op_comparable<T, op> && []() consteval {
			if constexpr (op == CompOp::eEqual)
				return requires(const T val) {noexcept(val == val);};
			else if constexpr (op == CompOp::eThreeWay)
				return requires(const T val) {noexcept(val <=> val);};
			else if constexpr (op == CompOp::eLess)
				return requires(const T val) {noexcept(val < val);};
			else if constexpr (op == CompOp::eLessOrEqual)
				return requires(const T val) {noexcept(val <= val);};
			else if constexpr (op == CompOp::eGreater)
				return requires(const T val) {noexcept(val > val);};
			else if constexpr (op == CompOp::eGreaterOrEqual)
				return requires(const T val) {noexcept(val >= val);};
			else
				flex::comptimeError("Invalid operation");
			return false;
		} ();

		template <typename T, CompOp op>
		concept op_refl_comparable = []() consteval {
			if constexpr (op == CompOp::eEqual)
				return requires(const T val) {::flex::reflection::equal(val, val);};
			else if constexpr (op == CompOp::eThreeWay)
				return requires(const T val) {::flex::reflection::three_way(val, val);};
			else if constexpr (op == CompOp::eLess)
				return requires(const T val) {::flex::reflection::less(val, val);};
			else if constexpr (op == CompOp::eLessOrEqual)
				return requires(const T val) {::flex::reflection::less_or_equal(val, val);};
			else if constexpr (op == CompOp::eGreater)
				return requires(const T val) {::flex::reflection::greater(val, val);};
			else if constexpr (op == CompOp::eGreaterOrEqual)
				return requires(const T val) {::flex::reflection::greater_or_equal(val, val);};
			else
				flex::comptimeError("Invalid operation");
			return false;
		} ();

		template <typename T, CompOp op>
		concept noexcept_op_refl_comparable = []() consteval {
			if constexpr (op == CompOp::eEqual)
				return requires(const T val) {noexcept(::flex::reflection::equal(val, val));};
			else if constexpr (op == CompOp::eThreeWay)
				return requires(const T val) {noexcept(::flex::reflection::three_way(val, val));};
			else if constexpr (op == CompOp::eLess)
				return requires(const T val) {noexcept(::flex::reflection::less(val, val));};
			else if constexpr (op == CompOp::eLessOrEqual)
				return requires(const T val) {noexcept(::flex::reflection::less_or_equal(val, val));};
			else if constexpr (op == CompOp::eGreater)
				return requires(const T val) {noexcept(::flex::reflection::greater(val, val));};
			else if constexpr (op == CompOp::eGreaterOrEqual)
				return requires(const T val) {noexcept(::flex::reflection::greater_or_equal(val, val));};
			else
				flex::comptimeError("Invalid operation");
			return false;
		} ();


		template <CompOp op>
		constexpr auto do_op_comparison(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
			noexcept (noexcept_op_comparable<typename std::remove_cvref<decltype(lhs)>::type, op>)
			requires op_comparable<typename std::remove_cvref<decltype(lhs)>::type, op>
		{
			if constexpr (op == CompOp::eEqual)
				return std::forward<decltype(lhs)> (lhs) == std::forward<decltype(rhs)> (rhs);
			else if constexpr (op == CompOp::eThreeWay)
				return std::forward<decltype(lhs)> (lhs) <=> std::forward<decltype(rhs)> (rhs);
			else if constexpr (op == CompOp::eLess)
				return std::forward<decltype(lhs)> (lhs) < std::forward<decltype(rhs)> (rhs);
			else if constexpr (op == CompOp::eLessOrEqual)
				return std::forward<decltype(lhs)> (lhs) <= std::forward<decltype(rhs)> (rhs);
			else if constexpr (op == CompOp::eGreater)
				return std::forward<decltype(lhs)> (lhs) > std::forward<decltype(rhs)> (rhs);
			else if constexpr (op == CompOp::eGreaterOrEqual)
				return std::forward<decltype(lhs)> (lhs) >= std::forward<decltype(rhs)> (rhs);
			else
				flex::comptimeError("Invalid operation");
		}

		template <CompOp op>
		constexpr auto do_op_refl_comparison(auto&& lhs, flex::forward_of<decltype(lhs)> auto&& rhs)
			noexcept (noexcept_op_refl_comparable<typename std::remove_cvref<decltype(lhs)>::type, op>)
			requires op_refl_comparable<typename std::remove_cvref<decltype(lhs)>::type, op>
		{
			if constexpr (op == CompOp::eEqual)
				return ::flex::reflection::equal(std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs));
			else if constexpr (op == CompOp::eThreeWay) {
				return ::flex::reflection::three_way(
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}
			else if constexpr (op == CompOp::eLess)
				return ::flex::reflection::less(std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs));
			else if constexpr (op == CompOp::eLessOrEqual) {
				return ::flex::reflection::less_or_equal(
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}
			else if constexpr (op == CompOp::eGreater) {
				return ::flex::reflection::greater(
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}
			else if constexpr (op == CompOp::eGreaterOrEqual) {
				return ::flex::reflection::greater_or_equal(
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}
			else
				flex::comptimeError("Invalid operation");
		}
	}


	template <typename T>
	struct Comparator {
		using type = T;
		static constexpr bool is_equal_comparable = internals::op_comparable<T, internals::CompOp::eEqual>;
		static constexpr bool is_noexcept_equal_comparable
			= internals::noexcept_op_comparable<T, internals::CompOp::eEqual>;
		static constexpr bool is_three_way_comparable = internals::op_comparable<T, internals::CompOp::eThreeWay>;
		static constexpr bool is_noexcept_three_way_comparable
			= internals::noexcept_op_comparable<T, internals::CompOp::eThreeWay>;
		static constexpr bool is_less_comparable = internals::op_comparable<T, internals::CompOp::eLess>;
		static constexpr bool is_noexcept_less_comparable
			= internals::noexcept_op_comparable<T, internals::CompOp::eLess>;
		static constexpr bool is_less_or_equal_comparable
			= internals::op_comparable<T, internals::CompOp::eLessOrEqual>;
		static constexpr bool is_noexcept_less_or_equal_comparable
			= internals::noexcept_op_comparable<T, internals::CompOp::eLessOrEqual>;
		static constexpr bool is_greater_comparable = internals::op_comparable<T, internals::CompOp::eGreater>;
		static constexpr bool is_noexcept_greater_comparable
			= internals::noexcept_op_comparable<T, internals::CompOp::eGreater>;
		static constexpr bool is_greater_or_equal_comparable
			= internals::op_comparable<T, internals::CompOp::eGreaterOrEqual>;
		static constexpr bool is_noexcept_greater_or_equal_comparable
			= internals::noexcept_op_comparable<T, internals::CompOp::eGreaterOrEqual>;

		static constexpr auto equal(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
			noexcept (is_noexcept_equal_comparable)
			-> bool
			requires is_equal_comparable
		{
			return lhs == rhs;
		}

		static constexpr auto three_way(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
			noexcept (is_noexcept_three_way_comparable)
			requires is_noexcept_three_way_comparable
		{
			return lhs <=> rhs;
		}

		static constexpr auto less(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
			noexcept (is_noexcept_less_comparable)
			-> bool
			requires is_less_comparable
		{
			return lhs < rhs;
		}

		static constexpr auto less_or_equal(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
			noexcept (is_noexcept_less_or_equal_comparable)
			-> bool
			requires is_less_or_equal_comparable
		{
			return lhs <= rhs;
		}

		static constexpr auto greater(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
			noexcept (is_noexcept_greater_comparable)
			-> bool
			requires is_greater_comparable
		{
			return lhs > rhs;
		}

		static constexpr auto greater_or_equal(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
			noexcept (is_noexcept_greater_or_equal_comparable)
			-> bool
			requires is_greater_or_equal_comparable
		{
			return lhs >= rhs;
		}
	};


	template <reflectable T>
	struct Comparator<T> {
		private:
			using ReflTraits = flex::reflection::reflection_traits<T>;

			// moved loop as extern function instead of lambda because clang does not want to consteval
			template <internals::CompOp op, decltype(ReflTraits::member_count) I = 0>
			static constexpr auto is_op_comparable_loop() -> bool {
				using Member = typename std::tuple_element<I, typename ReflTraits::member_types>::type;
				if constexpr (!flex::specialization_of<Member, flex::WriteOnly>
					&& !internals::op_refl_comparable<Member, op>
				)
					return false;
				if constexpr (I + 1 < ReflTraits::member_count)
					return is_op_comparable_loop<op, I + 1> ();
				else
					return true;
			};

			template <internals::CompOp op>
			static constexpr bool is_op_comparable_v = []() constexpr {
				if constexpr (internals::op_comparable<T, op>)
					return true;
				else {
					return is_op_comparable_loop<op> ();
				}
			} ();

			// moved loop as extern function instead of lambda because clang does not want to consteval
			template <internals::CompOp op, decltype(ReflTraits::member_count) I = 0>
			static constexpr auto is_noexcept_op_comparable_loop() -> bool {
				using Member = typename std::tuple_element<I, typename ReflTraits::member_types>::type;
				if constexpr (!flex::specialization_of<Member, flex::WriteOnly>
					&& !internals::noexcept_op_refl_comparable<Member, op>
				)
					return false;
				if constexpr (I + 1 < ReflTraits::member_count)
					return is_noexcept_op_comparable_loop<op, I + 1> ();
				else
					return true;
			}

			template <internals::CompOp op>
			static constexpr bool is_noexcept_op_comparable_v = []() constexpr {
				if constexpr (internals::noexcept_op_comparable<T, op>)
					return true;
				else {
					return is_noexcept_op_comparable_loop<op> ();
				}
			} ();

			template <internals::CompOp op>
			static constexpr auto do_op_comparison(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
				noexcept (is_noexcept_op_comparable_v<op>)
				requires (is_op_comparable_v<op>)
			{
				if constexpr (internals::noexcept_op_comparable<T, op>
					|| (internals::op_comparable<T, op> && !is_noexcept_op_comparable_v<op>)
				) {
					return internals::do_op_comparison<op> (
						std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
					);
				}
				else {
					bool result {true};
					flex::reflection::zipForeachMember([&result](const auto& lhs, const auto& rhs) constexpr -> bool {
						if (flex::reflection::equal(lhs, rhs))
							return true;
						result = false;
						return false;
					}, std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs));
					return result;
				}
			}


		public:
			static constexpr bool is_equal_comparable = is_op_comparable_v<internals::CompOp::eEqual>;
			static constexpr bool is_noexcept_equal_comparable = is_noexcept_op_comparable_v<internals::CompOp::eEqual>;
			static constexpr bool is_three_way_comparable = false;
			static constexpr bool is_noexcept_three_way_comparable = false;
			static constexpr bool is_less_comparable = is_op_comparable_v<internals::CompOp::eLess>;
			static constexpr bool is_noexcept_less_comparable = is_noexcept_op_comparable_v<internals::CompOp::eLess>;
			static constexpr bool is_less_or_equal_comparable = is_op_comparable_v<internals::CompOp::eLessOrEqual>;
			static constexpr bool is_noexcept_less_or_equal_comparable
				= is_noexcept_op_comparable_v<internals::CompOp::eLessOrEqual>;
			static constexpr bool is_greater_comparable = is_op_comparable_v<internals::CompOp::eGreater>;
			static constexpr bool is_noexcept_greater_comparable
				= is_noexcept_op_comparable_v<internals::CompOp::eGreater>;
			static constexpr bool is_greater_or_equal_comparable
				= is_op_comparable_v<internals::CompOp::eGreaterOrEqual>;
			static constexpr bool is_noexcept_greater_or_equal_comparable
				= is_noexcept_op_comparable_v<internals::CompOp::eGreaterOrEqual>;


			static constexpr auto equal(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
				noexcept (is_noexcept_equal_comparable)
				-> bool
				requires is_equal_comparable
			{
				return do_op_comparison<internals::CompOp::eEqual> (
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}

			static constexpr auto less(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
				noexcept (is_noexcept_less_comparable)
				-> bool
				requires is_less_comparable
			{
				return do_op_comparison<internals::CompOp::eLess> (
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}

			static constexpr auto less_or_equal(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
				noexcept (is_noexcept_less_or_equal_comparable)
				-> bool
				requires is_less_or_equal_comparable
			{
				return do_op_comparison<internals::CompOp::eLessOrEqual> (
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}

			static constexpr auto greater(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
				noexcept (is_noexcept_greater_comparable)
				-> bool
				requires is_greater_comparable
			{
				return do_op_comparison<internals::CompOp::eGreater> (
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}

			static constexpr auto greater_or_equal(flex::forward_of<T> auto&& lhs, flex::forward_of<T> auto&& rhs)
				noexcept (is_noexcept_greater_or_equal_comparable)
				-> bool
				requires is_greater_or_equal_comparable
			{
				return do_op_comparison<internals::CompOp::eGreaterOrEqual> (
					std::forward<decltype(lhs)> (lhs), std::forward<decltype(rhs)> (rhs)
				);
			}
	};
}
