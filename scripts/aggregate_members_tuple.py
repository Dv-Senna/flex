import sys
import os

if len(sys.argv) != 3:
    print("Invalid amount of args")
    exit(1)

output_path = sys.argv[1]
amount = int(sys.argv[2]) + 1

content = "#pragma once\n\n"

content += "#include <cstddef>\n"
content += "#include <tuple>\n"
content += "#include <type_traits>\n\n"

content += "#include \"flex/reflection/aggregateMembersCount.hpp\"\n\n\n"


content += f"#define FLEX_REFLECTION_MAX_AGGREGATE_MEMBERS_COUNT {amount - 1}\n\n"

content += "namespace flex::reflection {\n"
content += "\ttemplate <flex::aggregate T>\n"
content += "\tconstexpr auto makeAggregateMembersTuple(const T &value) {\n"
content += "\t\tconstexpr std::size_t size {flex::reflection::aggregate_members_count_v<T>};\n"
for val in range(1, amount):
    args = ",".join([f"m{i}" for i in range(0, val)])
    content += f"\t\tif constexpr (size == {val})" + " {\n"
    content += f"\t\t\tconst auto &[{args}] {{value}};\n"
    content += f"\t\t\treturn std::tie({args});\n"
    content += "\t\t}\n"
content += "\t}\n\n\n"


content += "\ttemplate <flex::aggregate T>\n"
content += "\tconstexpr auto makeAggregateMembersTuple(T &value) {\n"
content += "\t\tconstexpr std::size_t size {flex::reflection::aggregate_members_count_v<T>};\n"
for val in range(1, amount):
    args = ",".join([f"m{i}" for i in range(0, val)])
    content += f"\t\tif constexpr (size == {val})" + " {\n"
    content += f"\t\t\tauto &[{args}] {{value}};\n"
    content += f"\t\t\treturn std::tie({args});\n"
    content += "\t\t}\n"
content += "\t}\n\n"

content += "} // namespace flex::reflection"


os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as file:
    file.write(content)
