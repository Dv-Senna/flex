import sys
import os

if len(sys.argv) != 3:
    print("Invalid amount of argument")
    exit(-1)

output_path = sys.argv[1]
amount = int(sys.argv[2])

content = "#pragma once\n\n"

for level in range(2, amount+1):
    content += f"#define FLEX_MACROS_IOTA_FOR{level}(size, body, step, final, separator, ctx, start) "
    content += f"FLEX_MACROS_GUIDE_FOREACH{level}(body, step, final, separator, ctx, start, "
    content += f"FLEX_MACROS_MAKE_INT_GUIDE(size))"
    content += "\n"
    content += "\n"

    content += f"#define FLEX_MACROS_IOTA_FOR_BODY{level}(size, body, ctx) FLEX_MACROS_IOTA_FOR{level}(size, body, "
    content += f"FLEX_MACROS_NULL, FLEX_MACROS_NULL, FLEX_MACROS_NULL, ctx,)"
    content += "\n"

    content += f"#define FLEX_MACROS_IOTA_FOR_BODY_WITH_SEP{level}(size, body, separator, ctx) "
    content += f"FLEX_MACROS_IOTA_FOR{level}(size, body, FLEX_MACROS_NULL, FLEX_MACROS_NULL, separator, ctx,)"
    content += "\n"

    content += f"#define FLEX_MACROS_IOTA_FOR_STEP{level}(size, step, final, ctx, start) FLEX_MACROS_IOTA_FOR{level}"
    content += f"(size, FLEX_MACROS_NULL, step, final, FLEX_MACROS_NULL, ctx, start)"
    content += "\n"

    content += f"#define FLEX_MACROS_IOTA_FOR_STEP_WITH_SEP{level}(size, step, final, separator, ctx, start) "
    content += f"FLEX_MACROS_IOTA_FOR{level}(size, FLEX_MACROS_NULL, step, final, seperator, ctx, start)"
    content += "\n"
    content += "\n"
    content += "\n"


os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as file:
    file.write(content)
