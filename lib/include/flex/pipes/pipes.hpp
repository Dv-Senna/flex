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
	 *     - `T`, `U` some non-fundamental, cv-unqualified, non-reference types
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


	namespace __internals {
		template <typename PipeObject, typename EntryType>
		concept basic_pipe_header = std::same_as<PipeObject, std::remove_cvref_t<PipeObject>>
			&& std::same_as<EntryType, std::remove_cvref_t<EntryType>>
			&& !std::is_fundamental_v<EntryType>;

		template <typename PipeObject, typename EntryType>
		concept basic_pipe_body = requires(PipeObject &&pipe, EntryType entry) {
			pipe(entry);
			{entry | pipe} -> std::same_as<decltype(pipe(entry))>;
		};

		template <typename PipeObject, typename EntryType>
		concept incomplete_pipe = requires(PipeObject &&pipe, EntryType entry) {
			pipe(entry);
		};

	} // namespace __internals

	template <typename PipeObject, typename EntryType>
	concept const_rvalue_pipe = __internals::basic_pipe_header<PipeObject, EntryType>
		&& __internals::basic_pipe_body<PipeObject, const EntryType&&>;

	template <typename PipeObject, typename EntryType>
	concept rvalue_pipe = __internals::basic_pipe_header<PipeObject, EntryType>
		&& __internals::basic_pipe_body<PipeObject, EntryType&&>;

	template <typename PipeObject, typename EntryType>
	concept const_instance_pipe = __internals::basic_pipe_header<PipeObject, EntryType>
		&& __internals::basic_pipe_body<PipeObject, const EntryType&>;

	template <typename PipeObject, typename EntryType>
	concept instance_pipe = __internals::basic_pipe_header<PipeObject, EntryType>
		&& __internals::basic_pipe_body<PipeObject, EntryType&>;


	template <typename PipeObject, typename EntryType>
	concept pipe = const_rvalue_pipe<PipeObject, EntryType>
		|| rvalue_pipe<PipeObject, EntryType>
		|| const_instance_pipe<PipeObject, EntryType>
		|| instance_pipe<PipeObject, EntryType>;


	template <typename PipeObject>
	class BasicPipeObject {
		using This = BasicPipeObject<PipeObject>;
		public:
			constexpr BasicPipeObject() noexcept = default;
			constexpr ~BasicPipeObject() noexcept = default;

			BasicPipeObject(const This&) = delete;
			auto operator=(const This&) -> This& = delete;
			BasicPipeObject(This&&) = delete;
			auto operator=(This&&) -> This& = delete;
	};

	template <typename PipeObject, typename EntryType>
	requires __internals::incomplete_pipe<PipeObject, EntryType>
	constexpr auto operator|(EntryType &&entry, PipeObject &&pipeObject) noexcept {
		return pipeObject(std::forward<EntryType> (entry));
	}


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

			template <typename ...Args>
			requires std::constructible_from<PipeObject, Args...>
			constexpr auto operator()(Args&& ...args) const noexcept {
				return PipeObject{std::forward<Args> (args)...};
			}
	};

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

			template <typename ...Args>
			constexpr auto operator()(Args&& ...args) const noexcept {
				return TemplatedPipeObject{std::forward<Args> (args)...};
			}
	};

} // namespace flex::pipes
