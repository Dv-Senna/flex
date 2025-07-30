import sys
import os

if len(sys.argv) != 3:
    print("Invalid amount of argument")
    exit(-1)

output_path = sys.argv[1]
amount = int(sys.argv[2])

content = "#pragma once\n\n"

for level in range(2, amount+1):
    content += f"#define FLEX_MACROS_SEQ_FOREACH{level}(body, step, final, separator, ctx, start, seq) "
    content += f"FLEX_MACROS_GUIDE_FOREACH{level}(body, step, final, separator, ctx, start, "
    content += f"FLEX_MACROS_SEQ_TO_GUIDE(seq))"
    content += "\n"

    content += f"#define FLEX_MACROS_GUIDE_FOREACH{level}(body, step, final, separator, ctx, start, ...) "
    content += f"__FLEX_MACROS_GUIDE_FOREACH_FIRST{level}(body, step, final, separator, ctx, start, __VA_ARGS__"
    content += "\n"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_FIRST{level}(body, step, final, separator, ctx, step_ctx, ...) "
    content += f"__FLEX_MACROS_GUIDE_FOREACH_FIRST{level}_##__VA_ARGS__(body, step, final, separator, ctx, step_ctx)"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_FIRST{level}_(body, step, final, separator, ctx, step_ctx, ...) "
    content += f"__FLEX_MACROS_GUIDE_FOREACH_FIRST_B{level}(body, step, final, separator, ctx, step_ctx,"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_FIRST{level}___FLEX_GUIDE_END_MARK(body, step, final, "
    content += f"separator, ctx, step_ctx, ...) final(ctx, step_ctx)"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_FIRST_B{level}(body, step, final, separator, ctx, step_ctx, ...) "
    content += f"body(ctx, step_ctx, __VA_ARGS__) __FLEX_MACROS_GUIDE_FOREACH_A{level}(body, step, final, separator, "
    content += f"ctx, step(ctx, step_ctx), step_ctx,"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_A{level}(body, step, final, separator, ctx, step_ctx, "
    content += f"old_step_ctx, ...) __FLEX_MACROS_GUIDE_FOREACH_A{level}_##__VA_ARGS__(body, step, final, separator, "
    content += f"ctx, step_ctx, old_step)"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_A{level}_(body, step, final, separator, ctx, step_ctx, "
    content += f"old_step_ctx) __FLEX_MACROS_GUIDE_FOREACH_B{level}(body, step, final, separator, ctx, step_ctx, "
    content += "old_step_ctx,"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_A{level}___FLEX_GUIDE_END_MARK(body, step, final, separator, "
    content += f"ctx, step_ctx, old_step_ctx) final(ctx, step_ctx)"
    content += "\n"

    content += f"#define __FLEX_MACROS_GUIDE_FOREACH_B{level}(body, step, final, separator, ctx, step_ctx, "
    content += f"old_step_ctx, ...) separator(ctx, old_step_ctx) body(ctx, step_ctx, __VA_ARGS__) "
    content += f"__FLEX_MACROS_GUIDE_FOREACH_A{level}(body, step, final, separator, ctx, step(ctx, step_ctx), step_ctx,"
    content += "\n"
    content += "\n"


    content += f"#define FLEX_MACROS_SEQ_FOREACH_BODY{level}(body, ctx, seq) FLEX_MACROS_SEQ_FOREACH{level}(body, "
    content += f"FLEX_MACROS_NULL, FLEX_MACROS_NULL, FLEX_MACROS_NULL, ctx,, seq)"
    content += "\n"

    content += f"#define FLEX_MACROS_SEQ_FOREACH_BODY_WITH_SEP{level}(body, separator, ctx, seq) "
    content += f"FLEX_MACROS_SEQ_FOREACH{level}(body, FLEX_MACROS_NULL, FLEX_MACROS_NULL, separator, ctx,, seq)"
    content += "\n"

    content += f"#define FLEX_MACROS_SEQ_FOREACH_STEP{level}(step, final, ctx, start, seq) "
    content += f"FLEX_MACROS_SEQ_FOREACH{level}(FLEX_MACROS_NULL, step, final, FLEX_MACROS_NULL, ctx, start, seq)"
    content += "\n"

    content += f"#define FLEX_MACROS_SEQ_FOREACH_STEP_WITH_SEP{level}(step, final, separator, ctx, start, seq) "
    content += f"FLEX_MACROS_SEQ_FOREACH{level}(FLEX_MACROS_NULL, step, final, separator, ctx, start, seq)"
    content += "\n"
    content += "\n"
    content += "\n"


os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as file:
    file.write(content)
