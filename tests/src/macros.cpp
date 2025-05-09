#include <flex/macros/macros.hpp>
#include <flex/logger.hpp>


#define ENUM_BODY(ctx, step_ctx, ...) __VA_ARGS__ = step_ctx,
#define ENUM_STEP(ctx, step_ctx) step_ctx << 1
#define ENUM_FINAL(ctx, step_ctx) eBitMask = (step_ctx) - 1

#define MAKE_FLAG_ENUM(name, ...) enum class name : std::uint32_t {\
	FLEX_MACROS_SEQ_FOREACH(ENUM_BODY, ENUM_STEP, ENUM_FINAL, FLEX_MACROS_NULL,, 1, FLEX_MACROS_VAR_TO_SEQ(__VA_ARGS__))\
}

MAKE_FLAG_ENUM(Access,
	eRead,
	eWrite,
	eExecute
);


auto main(int, char**) -> int {
	flex::Logger::info("Access::eRead : {:b}", (std::uint32_t)Access::eRead);
	flex::Logger::info("Access::eWrite : {:b}", (std::uint32_t)Access::eWrite);
	flex::Logger::info("Access::eExecute: {:b}", (std::uint32_t)Access::eExecute);
	flex::Logger::info("Access::eBitMask : {:b}", (std::uint32_t)Access::eBitMask);
	return 0;
}
