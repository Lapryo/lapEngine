import os

project_name = input("Enter scene name (all lowercase, no spaces): ")

if not os.path.exists("bin"):
    os.makedirs("bin")

if not os.path.exists("bin/" + project_name):
    os.makedirs("bin/" + project_name)

if not os.path.exists("bin/" + project_name + "/objects"):
    os.makedirs("bin/" + project_name + "/objects")

with open("bin/" + project_name + "/" + project_name + ".json", "w") as f:
    f.write("{\n")
    f.write("\t\"name\": \"main\",\n")
    f.write("\t\"systems\": [],\n")
    f.write("\t\"instances\": {\n")
    f.write("\t\t\"prefabs\": [],\n")
    f.write("\t\t\"objects\": []\n")
    f.write("\t}\n")
    f.write("}")
