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
content += " * This macro increment the given integer, in a symbol-compatible way.\n"
content += " * */\n"
content += "#define FLEX_MACROS_INCREMENT(value) FLEX_MACROS_CONCAT(FLEX_MACROS_INCREMENT_, value)\n\n"

for val in range(0, amount+1):
    content += f"#define FLEX_MACROS_INCREMENT_{val} {val+1}\n"

content += "// NOLINTEND(cppcoreguidelines-macro-usage)"

os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as file:
    file.write(content)
