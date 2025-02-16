#pragma once


#define __FLEX_MACROS_RPARENTHESIS() )
#define FLEX_MACROS_EMPTY

#define FLEX_MACROS_NULL(...)
#define FLEX_MACROS_STATE(x, ...) __VA_ARGS__
#define FLEX_MACROS_COMMA(...) ,

#define FLEX_MACROS_STRINGIFY(...) __FLEX_MACROS_STRINGIFY_(__VA_ARGS__)
#define __FLEX_MACROS_STRINGIFY_(...) #__VA_ARGS__

/*
 * @brief Convert a sequence to a guide
 *
 * A sequence is defined as `(a)(b)(c)`. It's easy to write for a user, but it lacks
 * some iteratable properties that makes guide a better choice.
 *
 * A guide is defined as `)a) )b) )c) __FLEX_GUIDE_END_MARK)`. It's a pain to read
 * and write, but it allows for context passing with unconstrained iterations.
 *
 * This macro is here to get the best of both world, ease of write and ease of
 * iteration, by converting automaticly a sequence to a guide.
 *
 * The defintion of a guide is inspired by https://github.com/HolyBlackCat/macro_sequence_for
 * */
#define FLEX_MACROS_SEQ_TO_GUIDE(seq) __FLEX_MACROS_SEQ_TO_GUIDE_END(__FLEX_MACROS_SEQ_TO_GUIDE_A seq) __FLEX_GUIDE_END_MARK)
#define __FLEX_MACROS_SEQ_TO_GUIDE_A(...) __FLEX_MACROS_RPARENTHESIS FLEX_MACROS_EMPTY() __VA_ARGS__ __FLEX_MACROS_RPARENTHESIS FLEX_MACROS_EMPTY() __FLEX_MACROS_SEQ_TO_GUIDE_B
#define __FLEX_MACROS_SEQ_TO_GUIDE_B(...) __FLEX_MACROS_RPARENTHESIS FLEX_MACROS_EMPTY() __VA_ARGS__ __FLEX_MACROS_RPARENTHESIS FLEX_MACROS_EMPTY() __FLEX_MACROS_SEQ_TO_GUIDE_A
#define __FLEX_MACROS_SEQ_TO_GUIDE_A_END
#define __FLEX_MACROS_SEQ_TO_GUIDE_B_END
#define __FLEX_MACROS_SEQ_TO_GUIDE_END(...) __FLEX_MACROS_SEQ_TO_GUIDE_END_(__VA_ARGS__)
#define __FLEX_MACROS_SEQ_TO_GUIDE_END_(...) __VA_ARGS__##_END


/*
 * @brief Iterate over a sequence
 *
 * @param body(ctx, step_ctx, value) The body of the loop
 * @param step(ctx, step_ctx) The step statement
 * @param final(ctx, step_ctx) Statement inserted at the end, using the final step_ctx
 * @param separator(ctx, step_ctx) A statement inserted between each instance body
 * @param ctx A constant value passed to the macros
 * @param start The start value of step_ctx
 * @param seq The sequence to iterate upon
 *
 * This macro is equivalent to the following pseudo-code :
 * ```
 *   expr :=
 *   step_ctx := start
 *   for (value in seq)
 *       expr = expr body(ctx, step_ctx, element)
 *       if element is not last
 *		     expr = expr separator(ctx, step_ctx)
 *		 step_ctx = step(ctx, step_ctx)
 *
 *	 expr = expr final(ctx, step_ctx)
 *	 return expr
 * ```
 * */
#define FLEX_MACROS_SEQ_FOREACH(body, step, final, separator, ctx, start, seq) FLEX_MACROS_GUIDE_FOREACH(body, step, final, separator, ctx, start, FLEX_MACROS_SEQ_TO_GUIDE(seq))


#define FLEX_MACROS_GUIDE_FOREACH(body, step, final, separator, ctx, start, ...) __FLEX_MACROS_GUIDE_FOREACH_FIRST(body, step, final, separator, ctx, start, __VA_ARGS__

#define __FLEX_MACROS_GUIDE_FOREACH_FIRST(body, step, final, separator, ctx, step_ctx, ...) __FLEX_MACROS_GUIDE_FOREACH_FIRST_##__VA_ARGS__(body, step, final, separator, ctx, step_ctx)
#define __FLEX_MACROS_GUIDE_FOREACH_FIRST_(body, step, final, separator, ctx, step_ctx, ...) __FLEX_MACROS_GUIDE_FOREACH_FIRST_B(body, step, final, separator, ctx, step_ctx,
#define __FLEX_MACROS_GUIDE_FOREACH_FIRST___FLEX_GUIDE_END_MARK(body, step, final, separator, ctx, step_ctx, ...) final(ctx, step_ctx)
#define __FLEX_MACROS_GUIDE_FOREACH_FIRST_B(body, step, final, separator, ctx, step_ctx, ...) body(ctx, step_ctx, __VA_ARGS__)\
	__FLEX_MACROS_GUIDE_FOREACH_A(body, step, final, separator, ctx, step(ctx, step_ctx), step_ctx,

#define __FLEX_MACROS_GUIDE_FOREACH_A(body, step, final, separator, ctx, step_ctx, old_step_ctx, ...)\
	__FLEX_MACROS_GUIDE_FOREACH_A_##__VA_ARGS__(body, step, final, separator, ctx, step_ctx, old_step)
#define __FLEX_MACROS_GUIDE_FOREACH_A_(body, step, final, separator, ctx, step_ctx, old_step_ctx) __FLEX_MACROS_GUIDE_FOREACH_B(body, step, final, separator, ctx, step_ctx, old_step_ctx,
#define __FLEX_MACROS_GUIDE_FOREACH_A___FLEX_GUIDE_END_MARK(body, step, final, separator, ctx, step_ctx, old_step_ctx) final(ctx, step_ctx)
#define __FLEX_MACROS_GUIDE_FOREACH_B(body, step, final, separator, ctx, step_ctx, old_step_ctx, ...) separator(ctx, old_step_ctx)\
	body(ctx, step_ctx, __VA_ARGS__)\
	__FLEX_MACROS_GUIDE_FOREACH_A(body, step, final, separator, ctx, step(ctx, step_ctx), step_ctx,


#define FLEX_MACROS_SEQ_FOREACH_BODY(body, ctx, seq) FLEX_MACROS_SEQ_FOREACH(body, FLEX_MACROS_NULL, FLEX_MACROS_NULL, FLEX_MACROS_NULL, ctx,, seq)
#define FLEX_MACROS_SEQ_FOREACH_BODY_WITH_SEP(body, separator, ctx, seq) FLEX_MACROS_SEQ_FOREACH(body, FLEX_MACROS_NULL, FLEX_MACROS_NULL, separator, ctx,, seq)
#define FLEX_MACROS_SEQ_FOREACH_STEP(step, final, ctx, start, seq) FLEX_MACROS_SEQ_FOREACH(FLEX_MACROS_NULL, step, final, FLEX_MACROS_NULL, ctx, start, seq)
#define FLEX_MACROS_SEQ_FOREACH_STEP_WITH_SEP(step, final, separator, ctx, start, seq) FLEX_MACROS_SEQ_FOREACH(FLEX_MACROS_NULL, step, final, separator, ctx, start, seq)

