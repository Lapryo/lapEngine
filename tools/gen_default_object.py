import os

{
    "name": "camera",
    "parent": "",
    "child-index": -1,
    "elements": []
}

project_name = input("Enter object name (all lowercase, no spaces): ")

if not os.path.exists("bin"):
    os.makedirs("bin")

if not os.path.exists("bin/" + project_name):
    os.makedirs("bin/" + project_name)

if not os.path.exists("bin/" + project_name + "/elements"):
    os.makedirs("bin/" + project_name + "/elements")

with open("bin/" + project_name + "/" + project_name + ".json", "w") as f:
    f.write("{\n")
    f.write("\t\"name\": \"" + project_name + "\",\n")
    f.write("\t\"parent\": [],\n")
    f.write("\t\"child-index\": -1,\n")
    f.write("\t\"elements\": []\n")
    f.write("}")
