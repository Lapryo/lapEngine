import re

def pascal_to_kebab(s):
    # Insert a hyphen before any uppercase letter that is preceded by a lowercase letter or number.
    # This handles cases like 'APascalCaseString' -> 'A-Pascal-Case-String'
    s = re.sub(r'([a-z0-9])([A-Z])', r'\1-\2', s)
    # Convert the entire string to lowercase
    return s.lower()

def pascal_to_camel(pascal_case_string):
    if not pascal_case_string:
        return ""
    # Lowercase the first character and concatenate with the rest of the string
    return pascal_case_string[0].lower() + pascal_case_string[1:]

name = input("Enter object name: ")

camel_name = pascal_to_camel(name)

with open("editor/include/objects/" + camel_name + ".hpp", "a") as f:
    f.write("#pragma once\n\n")
    f.write("#include \"core.hpp\"\n\n")
    f.write("namespace " + name + "Object\n")
    f.write("{\n")
    f.write("\tvoid RegisterLogic();\n")
    f.write("}\n")

kebab_name = pascal_to_kebab(name)

with open("editor/source/objects/" + camel_name + ".cpp", "a") as f:
    f.write("#include \"objects/" + camel_name + ".hpp\"\n\n")
    f.write("void " + name + "Object::RegisterLogic()\n")
    f.write("{\n")
    f.write("\tScriptRegistry::onCreateFunctions[\"" + kebab_name + "\"] = [](Scene *scene, Object object)\n")
    f.write("\t{\n")
    f.write("\t\t\n")
    f.write("\t};\n")
    f.write("}\n")

with open("editor/include/objects.hpp", "a") as f:
    f.write("\n#include \"objects/" + camel_name + ".hpp\"")