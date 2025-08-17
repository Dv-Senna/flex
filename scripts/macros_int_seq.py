import sys
import os

if len(sys.argv) != 3:
    print("Invalid amount of argument")
    exit(-1)

output_path = sys.argv[1]
amount = int(sys.argv[2])

content = "#pragma once\n\n"
content += "// NOLINTBEGIN(cppcoreguidelines-macro-usage)\n\n"

#content += "\n"
content += "/*\n"
content += " * This macro create a seq composed of increasing integer, in range [0, size).\n"
content += " * */\n"
content += "#define FLEX_MACROS_MAKE_INT_SEQ(size) FLEX_MACROS_CONCAT(FLEX_MACROS_MAKE_INT_SEQ_, size)\n\n"

content += "#define FLEX_MACROS_MAKE_INT_SEQ_0\n"
for val in range(1, amount+1):
    content += f"#define FLEX_MACROS_MAKE_INT_SEQ_{val} FLEX_MACROS_MAKE_INT_SEQ_{val-1} ({val-1})\n"


content += "\n"
content += "/*\n"
content += " * This macro create a guide composed of increasing integer, in range [0, size).\n"
content += " * */\n"
content += "#define FLEX_MACROS_MAKE_INT_GUIDE(size) FLEX_MACROS_CONCAT(FLEX_MACROS_MAKE_INT_GUIDE_, size) "
content += "__FLEX_GUIDE_END_MARK)\n\n"

content += "#define FLEX_MACROS_MAKE_INT_GUIDE_0\n"
for val in range(1, amount+1):
    content += f"#define FLEX_MACROS_MAKE_INT_GUIDE_{val} FLEX_MACROS_MAKE_INT_GUIDE_{val-1} ){val-1})\n"

content += "// NOLINTEND(cppcoreguidelines-macro-usage)"

os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as file:
    file.write(content)
