#pragma once

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define FLEX_MACROS_IOTA_FOR(size, body, step, final, separator, ctx, start) \
	FLEX_MACROS_GUIDE_FOREACH(body, step, final, separator, ctx, start, FLEX_MACROS_MAKE_INT_GUIDE(size))

#define FLEX_MACROS_IOTA_FOR_BODY(size, body, ctx) \
	FLEX_MACROS_IOTA_FOR(size, body, FLEX_MACROS_NULL, FLEX_MACROS_NULL, FLEX_MACROS_NULL, ctx,)
#define FLEX_MACROS_IOTA_FOR_BODY_WITH_SEP(size, body, separator, ctx) \
	FLEX_MACROS_IOTA_FOR(size, body, FLEX_MACROS_NULL, FLEX_MACROS_NULL, separator, ctx,)
#define FLEX_MACROS_IOTA_FOR_STEP(size, step, final, ctx, start) \
	FLEX_MACROS_IOTA_FOR(size, FLEX_MACROS_NULL, step, final, FLEX_MACROS_NULL, ctx, start)
#define FLEX_MACROS_IOTA_FOR_STEP_WITH_SEP(size, step, final, separator, ctx, start) \
	FLEX_MACROS_IOTA_FOR(size, FLEX_MACROS_NULL, step, final, seperator, ctx, start)

// NOLINTEND(cppcoreguidelines-macro-usage)
