#pragma once

#include "flex/typeTraits.hpp"


namespace flex::operators {
	template <typename T>
	concept dereferenceable = flex::pointer<T> || requires(T val) {*val;};

	template <typename T>
	concept addressable = requires(T val) {val.operator&();};

	template <typename T>
	concept member_dereferenceable = flex::pointer<T> || requires(T val) {val.operator->();};

	template <typename T, typename ...Args>
	concept subscript_accessible = requires(T val) {val[std::declval<Args> ()...];};


	template <typename T>
	concept equality_comparable = requires(T val) {val == val;};

	template <typename T>
	concept three_way_comparable = requires(T val) {val <=> val;};


	template <typename T>
	concept sumable = requires(T val) {val + val;};

	template <typename T>
	concept sum_assignable = requires(T val) {val += val;};

	template <typename T>
	concept substractable = requires(T val) {val - val;};

	template <typename T>
	concept sub_assignable = requires(T val) {val -= val;};

	template <typename T>
	concept multipliable = requires(T val) {val * val;};

	template <typename T>
	concept multiply_assignable = requires(T val) {val *= val;};

	template <typename T>
	concept dividable = requires(T val) {val / val;};

	template <typename T>
	concept divide_assignable = requires(T val) {val /= val;};

	template <typename T>
	concept pre_incrementable = requires(T val) {++val;};

	template <typename T>
	concept post_incrementable = requires(T val) {val++;};

	template <typename T>
	concept pre_decrementable = requires(T val) {--val;};

	template <typename T>
	concept post_decrementable = requires(T val) {val--;};


	template <typename T, typename U>
	concept right_sumable_with = requires(T val, U val2) {val + val2;};

	template <typename T, typename U>
	concept left_sumable_with = requires(T val, U val2) {val2 + val;};

	template <typename T, typename U>
	concept sum_assignable_with = requires(T val, U val2) {val += val2;};

	template <typename T, typename U>
	concept right_substractable_with = requires(T val, U val2) {val + val2;};

	template <typename T, typename U>
	concept left_substractable_with = requires(T val, U val2) {val2 + val;};

	template <typename T, typename U>
	concept sub_assignable_with = requires(T val, U val2) {val += val2;};

	template <typename T, typename U>
	concept right_multipliable_with = requires(T val, U val2) {val + val2;};

	template <typename T, typename U>
	concept left_multipliable_with = requires(T val, U val2) {val2 + val;};

	template <typename T, typename U>
	concept multiply_assignable_with = requires(T val, U val2) {val += val2;};

	template <typename T, typename U>
	concept right_dividable_with = requires(T val, U val2) {val + val2;};

	template <typename T, typename U>
	concept left_dividable_with = requires(T val, U val2) {val2 + val;};

	template <typename T, typename U>
	concept divide_assignable_with = requires(T val, U val2) {val += val2;};


	template <typename T, typename ...Args>
	concept callable = requires(T val) {val(std::declval<Args> ()...);};

} // namespace flex::operators
