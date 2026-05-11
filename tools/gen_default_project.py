import os

project_name = input("Enter project name: ")

if not os.path.exists("bin"):
    os.makedirs("bin")

if not os.path.exists("bin/" + project_name + "_project"):
    os.makedirs("bin/" + project_name + "_project")

with open("bin/" + project_name + "_project/project.json", "w") as f:
    f.write("{\n")
    f.write("\t\"name\": \"" + project_name + "\",\n")
    f.write("\t\"version\": \"0.0.1-1\",\n")
    f.write("\t\"assets\": [],\n")
    f.write("\t\"scenes\": [\n")
    f.write("\t\t{\n")
    f.write("\t\t\t\"name\": \"main\",\n")
    f.write("\t\t\t\"systems\": [],\n")
    f.write("\t\t\t\"instances\": {\n")
    f.write("\t\t\t\t\"prefabs\": [],\n")
    f.write("\t\t\t\t\"objects\": []\n")
    f.write("\t\t\t}\n")
    f.write("\t\t}\n")
    f.write("\t],\n")
    f.write("\t\"prefabs\": []\n")
    f.write("}")
