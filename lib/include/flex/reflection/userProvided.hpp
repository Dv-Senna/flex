#pragma once

#include <algorithm>
#include <array>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "flex/containers/inplaceVector.hpp"
#include "flex/core/typeSets.hpp"
#include "flex/core/typeTraits.hpp"
#include "flex/reflection/aggregate.hpp"


namespace flex::reflection::userProvided {
	template <typename T>
	concept has_metadata = requires {
		typename T::FlexMetadata;
	};

	namespace internals {
		template <
			flex::tuple T,
			std::size_t I = 0,
			std::size_t N = std::tuple_size<T>::value
		>
		struct is_metadata_rename_valid : std::bool_constant<
			flex::tuple<std::tuple_element_t<I, T>>
			&& std::tuple_size<std::tuple_element_t<I, T>>::value == 2
			&& flex::string<typename std::tuple_element<0, std::tuple_element_t<I, T>>::type>
			&& std::is_member_object_pointer<typename std::tuple_element<1, std::tuple_element_t<I, T>>::type>::value
			&& is_metadata_rename_valid<T, I + 1, N>::value
		> {};

		template <typename T, std::size_t N>
		struct is_metadata_rename_valid<T, N, N> : std::true_type {};


		template <
			flex::tuple T,
			std::size_t I = 0,
			std::size_t N = std::tuple_size<T>::value
		>
		struct is_metadata_remove_valid : std::bool_constant<
			std::is_member_object_pointer<std::tuple_element_t<I, T>>::value
			&& is_metadata_remove_valid<T, I + 1, N>::value
		> {};

		template <typename T, std::size_t N>
		struct is_metadata_remove_valid<T, N, N> : std::true_type {};


		template <typename T>
		struct is_getter;// : std::false_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_getter<T (S::*)() const> : std::true_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_getter<T (S::*)() const noexcept> : std::true_type {};

		template <typename T>
		concept getter = is_getter<typename std::remove_cvref<T>::type>::value;


		template <typename T>
		struct is_setter;// : std::false_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_setter<void (S::*)(T)> : std::true_type {};

		template <typename S, typename T>
		requires (!std::is_void<T>::value)
		struct is_setter<void (S::*)(T) noexcept> : std::true_type {};

		template <typename T>
		concept setter = is_setter<typename std::remove_cvref<T>::type>::value;


		template <typename Getter, typename Setter>
		struct is_getter_setter_pair;// : std::false_type {};

		template <getter Getter, setter Setter>
		struct is_getter_setter_pair<Getter, Setter> : std::bool_constant<
			std::invocable<
				flex::member_pointer_extractor_t<Setter>,
				flex::extract_signature_return_t<flex::member_pointer_extractor_t<Getter>>
			>
		> {};

		template <setter Setter, getter Getter>
		struct is_getter_setter_pair<Setter, Getter> : is_getter_setter_pair<Getter, Setter> {};


		template <typename T, std::size_t N = std::tuple_size<T>::value>
		struct is_metadata_new_member_field_valid : std::false_type {};

		template <typename T>
		struct is_metadata_new_member_field_valid<T, 2> : std::bool_constant<
			flex::tuple<T>
			&& flex::string<typename std::tuple_element<0, T>::type>
			&& (
				getter<typename std::tuple_element<1, T>::type>
				|| setter<typename std::tuple_element<1, T>::type>
				|| std::is_member_object_pointer_v<typename std::tuple_element<1, T>::type>
			)
		> {};

		template <typename T>
		struct is_metadata_new_member_field_valid<T, 3> : std::bool_constant<
			flex::tuple<T>
			&& flex::string<typename std::tuple_element<0, T>::type>
			&& is_getter_setter_pair<
				typename std::tuple_element<1, T>::type,
				typename std::tuple_element<2, T>::type
			>::value
		> {};


		template <
			flex::tuple T,
			std::size_t I = 0,
			std::size_t N = std::tuple_size<T>::value
		>
		struct is_metadata_new_member_valid : std::bool_constant<
			is_metadata_new_member_field_valid<std::tuple_element_t<I, T>>::value
			&& is_metadata_new_member_valid<T, I + 1, N>::value
		> {};

		template <typename T, std::size_t N>
		struct is_metadata_new_member_valid<T, N, N> : std::true_type {};
	}


	template <typename T>
	concept metadata_has_rename = requires {
		{T::rename} -> flex::tuple;
	}
		&& std::tuple_size<decltype(T::rename)>::value > 0
		&& internals::is_metadata_rename_valid<decltype(T::rename)>::value;

	template <typename T>
	concept metadata_has_remove = requires {
		{T::remove} -> flex::tuple;
	}
		&& std::tuple_size<decltype(T::remove)>::value > 0
		&& internals::is_metadata_remove_valid<decltype(T::remove)>::value;

	template <typename T>
	concept metadata_has_new_member = requires {
		{T::new_member} -> flex::tuple;
	}
		&& std::tuple_size<decltype(T::new_member)>::value > 0
		&& internals::is_metadata_new_member_valid<decltype(T::new_member)>::value;


	template <typename T>
	concept has_valid_metadata = has_metadata<T>
		&& (
			flex::aggregate<T>
			|| (!metadata_has_remove<T> && !metadata_has_rename<T>)
		);


	
	namespace internals {
		template <typename S, internals::getter auto getter = nullptr, internals::setter auto setter = nullptr>
		class MemberWrapper {
			using This = MemberWrapper<S, getter, setter>;
			public:
				MemberWrapper() = delete;
				MemberWrapper(const This&) noexcept = delete;
				auto operator=(const This&) noexcept -> This& = delete;
				auto operator=(This&&) noexcept -> This& = delete;

				constexpr MemberWrapper(S& instance) noexcept : m_instance {&instance} {}
				constexpr MemberWrapper(This&&) noexcept = default;

				constexpr auto operator=(auto&& value) const
					noexcept(m_instance->*setter(std::declval<decltype(value)> ()))
					requires (setter != nullptr)
				{
					m_instance->*setter(value);
				}

				constexpr operator flex::extract_signature_return_t<
					flex::member_pointer_extractor_t<decltype(getter)>
				> () const
					noexcept(m_instance->*getter())
					requires (getter != nullptr)
				{
					return m_instance->*getter();
				}

				constexpr auto operator->() const
					noexcept(m_instance->*getter())
					-> typename std::add_pointer<flex::extract_signature_return_t<
						flex::member_pointer_extractor_t<decltype(getter)>
					>>::type
					requires (getter != nullptr)
				{
					using Result = flex::extract_signature_return_t<
						flex::member_pointer_extractor_t<decltype(getter)>
					>;
					struct PointerWrapper {
						Result result;
						constexpr auto operator->() noexcept -> Result& {return result;}
					};
					if constexpr (std::is_reference<Result>::value)
						return &m_instance->*getter();
					else
						return PointerWrapper{m_instance->*getter()};
				}

			private:
				S* m_instance;
		};


		template <flex::aggregate T, std::size_t I>
		consteval auto removeMember() noexcept -> bool {
			constexpr auto& currentMember {std::get<I> (flex::reflection::aggregate::getMemberTie(
				flex::reflection::aggregate::internals::fakeObject<T>
			))};
			auto loop {[&] <std::size_t J = 0> (auto& loop) {
				constexpr auto& currentRemove {std::get<J> (T::FlexMetadata::remove)};
				if constexpr (static_cast<const void*> (&currentMember) == static_cast<const void*> (
					&(flex::reflection::aggregate::internals::fakeObject<T>.*currentRemove)
				))
					return true;
				if constexpr (J + 1 < std::tuple_size_v<decltype(T::FlexMetadata::remove)>)
					return loop.template operator() <J + 1> (loop);
				else
					return false;
			}};
			return loop(loop);
		}


		template <flex::aggregate T, std::size_t I>
		consteval auto renameMember() noexcept -> std::optional<std::string_view> {
			constexpr auto& currentMember {std::get<I> (flex::reflection::aggregate::getMemberTie(
				flex::reflection::aggregate::internals::fakeObject<T>
			))};
			auto loop {[&] <std::size_t J = 0> (auto& loop) -> std::optional<std::string_view> {
					constexpr std::string_view currentRenameName {std::get<0> (std::get<J> (T::FlexMetadata::rename))};
					constexpr auto& currentRenameMember {std::get<1> (std::get<J> (T::FlexMetadata::rename))};
				if constexpr (static_cast<const void*> (&currentMember) == static_cast<const void*> (
					&(flex::reflection::aggregate::internals::fakeObject<T>.*currentRenameMember)
				))
					return currentRenameName;
				if constexpr (J + 1 < std::tuple_size_v<decltype(T::FlexMetadata::rename)>)
					return loop.template operator() <J + 1> (loop);
				else
					return std::nullopt;
			}};
			return loop(loop);
		}


		template <flex::aggregate T>
		consteval auto processAggregateMemberNames(auto names) noexcept {
			constexpr auto aggregateMemberNames {flex::reflection::aggregate::getMemberNames<T> ()};
			names.appendRange(aggregateMemberNames);

			auto removeLoop {[&names] <std::size_t I = 0> (auto& removeLoop) noexcept {
				constexpr std::tuple aggregateMembers {flex::reflection::aggregate::getMemberTie(
					flex::reflection::aggregate::internals::fakeObject<T>
				)};
				if constexpr (removeMember<T, I> ())
					names.erase(std::ranges::remove(names, flex::reflection::aggregate::getMemberNames<T> ()[I]));
				if constexpr (I + 1 < std::tuple_size_v<decltype(aggregateMembers)>)
					removeLoop.template operator() <I + 1> (removeLoop);
			}};
			removeLoop(removeLoop);

			auto renameLoop {[&names] <std::size_t I = 0> (auto& renameLoop) noexcept {
				constexpr std::tuple aggregateMembers {flex::reflection::aggregate::getMemberTie(
					flex::reflection::aggregate::internals::fakeObject<T>
				)};
				constexpr std::optional renameData {renameMember<T, I> ()};
				if constexpr (renameData) {
					auto it {std::ranges::find(names, flex::reflection::aggregate::getMemberNames<T> ()[I])};
					assert(it != names.end() && "Can't rename member that was removed");
					*it = *renameData;
				}
				if constexpr (I + 1 < std::tuple_size_v<decltype(aggregateMembers)>)
					renameLoop.template operator() <I + 1> (renameLoop);
			}};
			renameLoop(renameLoop);
			return names;
		};


		template <has_valid_metadata T>
		consteval auto getMemberNames() noexcept {
			constexpr std::size_t namesCapacity {
				flex::reflection::aggregate::member_count_v<T>
				+ std::tuple_size_v<decltype(T::FlexMetadata::new_member)>
			};
			flex::containers::InplaceVector<std::string_view, namesCapacity, true> names {};
			if constexpr (flex::aggregate<T>)
				names = processAggregateMemberNames<T> (names);

			auto newMemberLoop {[&names] <std::size_t I = 0> (auto& newMemberLoop) noexcept {
				constexpr auto currentNewMember {std::get<I> (T::FlexMetadata::new_member)};
				names.pushBack(std::get<0> (currentNewMember));
				if constexpr (I + 1 < std::tuple_size_v<decltype(T::FlexMetadata::new_member)>)
					newMemberLoop.template operator() <I + 1> (newMemberLoop);
			}};
			newMemberLoop(newMemberLoop);
			return names;
		}
	}


	template <has_valid_metadata T>
	consteval auto getMemberNames() noexcept {
		constexpr auto names {internals::getMemberNames<T> ()};
		std::array<std::string_view, names.size()> namesAsArray {};
		std::ranges::copy(names, namesAsArray.begin());
		return namesAsArray;
	}



	namespace internals {
		template <
			has_valid_metadata T,
			std::size_t I = 0,
			std::size_t N = flex::reflection::aggregate::member_count<T>::value
		>
		struct get_processed_aggregate_member_types : flex::type_constant<typename flex::merge_tuple<
			typename std::conditional<internals::removeMember<T, I> (),
				std::tuple<>,
				std::tuple<typename std::tuple_element<I, flex::reflection::aggregate::get_member_types_t<T>>::type>
			>::type,
			typename get_processed_aggregate_member_types<T, I + 1, N>::type
		>::type> {};

		template <typename T, std::size_t N>
		struct get_processed_aggregate_member_types<T, N, N> : flex::type_constant<std::tuple<>> {};


		template <typename T, std::size_t N = std::tuple_size_v<T>, typename = void>
		requires (internals::is_metadata_new_member_field_valid<T, N>::value)
		struct get_new_member_type;

		template <typename T>
		struct get_new_member_type<T, 2,
			std::enable_if_t<std::is_member_object_pointer_v<typename std::tuple_element<1, T>::type>, void>
		> : flex::type_constant<
			flex::member_pointer_extractor_t<typename std::tuple_element<1, T>::type>
		> {};

		template <typename T>
		struct get_new_member_type<T, 2,
			typename std::enable_if<getter<typename std::tuple_element<1, T>::type>, void>::type
		> : flex::type_constant<
			typename std::add_const<typename std::remove_reference<
				flex::extract_signature_return_t<flex::member_pointer_extractor_t<
					typename std::tuple_element<1, T>::type
				>>
			>::type>::type
		> {};

		template <typename T>
		struct get_new_member_type<T, 2,
			typename std::enable_if<setter<typename std::tuple_element<1, T>::type>, void>::type
		> : flex::type_constant<
			flex::WriteOnly<typename std::remove_cvref<
				flex::extract_signature_argument_t<flex::member_pointer_extractor_t<
					typename std::tuple_element<1, T>::type
				>, 0>
			>::type>
		> {};

		template <typename T>
		struct get_new_member_type<T, 3> : flex::type_constant<
			typename std::remove_cvref<
				flex::extract_signature_return_t<flex::member_pointer_extractor_t<
					typename std::conditional<getter<typename std::tuple_element<1, T>::type>,
						typename std::tuple_element<1, T>::type,
						typename std::tuple_element<2, T>::type
					>::type
				>>
			>::type
		> {};


		template <
			has_valid_metadata T,
			std::size_t I = 0,
			std::size_t N = std::tuple_size<decltype(T::FlexMetadata::new_member)>::value
		>
		struct get_new_member_types : flex::type_constant<
			typename flex::merge_tuple<
				std::tuple<typename get_new_member_type<
					typename std::tuple_element<I, decltype(T::FlexMetadata::new_member)>::type
				>::type>,
				typename get_new_member_types<T, I + 1, N>::type
			>::type
		> {};

		template <typename T, std::size_t N>
		struct get_new_member_types<T, N, N> : flex::type_constant<std::tuple<>> {};
	}


	template <has_valid_metadata T>
	using get_member_types_t = typename flex::merge_tuple<
		typename internals::get_processed_aggregate_member_types<T>::type,
		typename internals::get_new_member_types<T>::type
	>::type;

/*	template <has_valid_metadata T, std::size_t I>
	constexpr auto getMember(flex::variant_of<T> auto& instance) noexcept {
		
	}*/
}
