#pragma once

#include <concepts>
#include <type_traits>
#include <utility>


namespace flex::pipes {
	/**
	 * @defgroup PipeObject Named Requirements: *PipeObject*
	 * @brief Object that can be used in pipe expression
	 *
	 * ### Requirements
	 * Given
	 *     - `T`, `U` some cv-unqualified, non-reference types
	 *     - `P`, `Q` some *PipeObject*
	 *     - `cp` an rvalue of `P`
	 *     - `cq` an rvalue of `Q`
	 *     - `cv` a constant instance or rvalue of `T`
	 * 
	 * <table>
	 *	<caption>Operations</caption>
	 *		<tr>
	 *			<th>Expression</th>
	 *			<th>Return type</th>
	 *			<th>Requirements</th>
	 *		</tr>
	 *		<tr>
	 *			<td>`cp(cv)`</td>
	 *			<td>(unspecified)</td>
	 *			<td rowspace="2">
	 *				Those two expression are equivalent. That is one of them
	 *				is just an other name for the other one.
	 *				With equivalent parameters, the result of those expressions
	 *				**must** remains the same in time.
	 *			</td>
	 *		</tr>
	 *		<tr>
	 *			<td>`cv | cp`</td>
	 *			<td>(unspecified)</td>
	 *		</tr>
	 * </table>
	 *
	 * ### Notes
	 *     - We call `T` the *EntryType* of the *PipeObject* `P`
	 * */


	template <typename PipeObject>
	class BasicPipeObject;

	namespace __internals {
		template <typename PipeObject, typename EntryType>
		concept incomplete_pipe = requires(PipeObject pipe, EntryType entry) {
			pipe(entry);
		};

	} // namespace __internals


	/**
	 * @ingroup PipeObject
	 * @brief A concept that modelise pipe object
	 *
	 * @warning Broken for now, as `entry | pipe` is considered invalid even when valid
	 * */
	template <typename PipeObject, typename EntryType>
	concept pipe = requires(PipeObject &&pipe, EntryType entry) {
		pipe(entry);
		{entry | pipe} -> std::same_as<decltype(pipe(entry))>;
	};


	/**
	 * @ingroup PipeObject
	 * @brief An overloading of pipe operator for incomplete pipe, that is pipe with only operator()
	 * */
	template <typename EntryType, __internals::incomplete_pipe<EntryType> PipeObject>
	constexpr auto operator|(EntryType &&entry, PipeObject &&pipeObject) noexcept {
		return pipeObject(std::forward<EntryType> (entry));
	}


	/**
	 * @brief A wrapper type that allow the construction of *PipeObject*
	 * @sa PipeObject
	 * */
	template <typename PipeObject>
	class PipeAdaptator final {
		using This = PipeAdaptator<PipeObject>;
		public:
			constexpr PipeAdaptator() noexcept = default;
			constexpr ~PipeAdaptator() noexcept = default;

			PipeAdaptator(const This&) = delete;
			auto operator=(const This&) -> This& = delete;
			PipeAdaptator(This&&) = delete;
			auto operator=(This&&) -> This& = delete;

			/**
			 * @ingroup PipeObject
			 * @brief Construct the `PipeObject` from the given arguments
			 * @param args The argument forwarded to the *PipeObject* constructor
			 * */
			template <typename ...Args>
			requires std::constructible_from<PipeObject, Args...>
			constexpr auto operator()(Args&& ...args) const noexcept {
				return PipeObject{std::forward<Args> (args)...};
			}
	};

	/**
	 * @brief A wrapper type that allow the construction of templated *PipeObject*
	 * @sa PipeObject
	 * */
	template <template <typename...> typename TemplatedPipeObject>
	class TemplatedPipeAdaptator final {
		using This = TemplatedPipeAdaptator<TemplatedPipeObject>;
		public:
			constexpr TemplatedPipeAdaptator() noexcept = default;
			constexpr ~TemplatedPipeAdaptator() noexcept = default;

			TemplatedPipeAdaptator(const This&) = delete;
			auto operator=(const This&) -> This& = delete;
			TemplatedPipeAdaptator(This&&) = delete;
			auto operator=(This&&) -> This& = delete;

			/**
			 * @ingroup PipeObject
			 * @brief Construct the `PipeObject` from the given arguments
			 * @param args The argument forwarded to the *PipeObject* constructor
			 * @note The template parameters of the given *PipeObject* are deduced from
			 *       `std::forward<Args> (args)...`, so remember to provide deduction
			 *       guide if needed.
			 * */
			template <typename ...Args>
			constexpr auto operator()(Args&& ...args) const noexcept {
				return TemplatedPipeObject{std::forward<Args> (args)...};
			}
	};

} // namespace flex::pipes
