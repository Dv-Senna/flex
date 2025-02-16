#include <flex/macros/foreach.hpp>

#include <print>

#define ENUM_BODY(ctx, step_ctx, ...) __VA_ARGS__ = step_ctx,
#define ENUM_STEP(ctx, step_ctx) step_ctx << 1
#define ENUM_FINAL(ctx, step_ctx) eBitMask = (step_ctx) - 1

#define MAKE_FLAG_ENUM(name, seq) enum class name : std::uint32_t {FLEX_MACROS_SEQ_FOREACH(ENUM_BODY, ENUM_STEP, ENUM_FINAL, FLEX_MACROS_NULL,, 1, seq)}

MAKE_FLAG_ENUM(Access,
	(eRead)(eWrite)(eExecute)
);


auto main(int, char**) -> int {
	std::println("Access::eRead : {:b}", (std::uint32_t)Access::eRead);
	std::println("Access::eWrite : {:b}", (std::uint32_t)Access::eWrite);
	std::println("Access::eExecute: {:b}", (std::uint32_t)Access::eExecute);
	std::println("Access::eBitMask : {:b}", (std::uint32_t)Access::eBitMask);
	return 0;
}
