#include <cstdint>
#include <utility>

#include <flex/macros/macros.hpp>


#define PASS_THROUGH_BODY(ctx, step_ctx, ...) __VA_ARGS__
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


#define INNER_BODY(ctx, step_ctx, ...) __VA_ARGS__
#define OUTER_BODY(ctx, step_ctx, ...) FLEX_MACROS_IOTA_FOR_BODY2(__VA_ARGS__, INNER_BODY, FLEX_MACROS_NULL)

static_assert(std::to_underlying(Access::eRead) == 0b001);
static_assert(std::to_underlying(Access::eWrite) == 0b010);
static_assert(std::to_underlying(Access::eExecute) == 0b100);
static_assert(std::to_underlying(Access::eBitMask) == 0b111);
