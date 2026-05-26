import os
import sys

# TODO: IMPLEMENT ALL ELEMENT TYPES IN HERE

element_num = input("Enter element #:\n1. Origin2D\n2. Physics2D\n3. Frame\n4. UIList\n5. Sprite\n6. lapImage\n7. TextLabel\n8. EventBus\n9. UIButton\n10. Cam2D\n11. Script\n\n")
element_type = ""


match element_num:
    case "1":
        element_type = "origin-2d"
    case "2":
        element_type = "physics-2d"
    case "3":
        element_type = "frame"
    case "4":
        element_type = "ui-list"
    case "5":
        element_type = "sprite"
    case "6":
        element_type = "image"
    case "7":
        element_type = "text-label"
    case "8":
        element_type = "event-bus"
    case "9":
        element_type = "ui-button"
    case "10":
        element_type = "cam-2d"
    case "11":
        element_type = "script"
    case _:
        sys.exit("Wrong element # input")
    

if not os.path.exists("bin"):
    os.makedirs("bin")

with open("bin/element.json", "w") as f:
    f.write("{\n")
    f.write("\t\"type\": \"" + element_type + "\",\n")
    f.write("\t\"data\": {\n")
    match element_type:
        case "origin-2d":
            f.write("\t\t\"position\": [0, 0],\n")
            f.write("\t\t\"scale\": [1, 1]\n")
        case "physics-2d":
            f.write("\t\t\"body\": {\n")
            f.write("\t\t\t\"type\": \"static\"\n")
            f.write("\t\t}\n")
        case "frame":
            f.write("\t\t\"renderable\": {\n")
            f.write("\t\t\t\"is-screen-space\": true,\n")
            f.write("\t\t\t\"visible\": true,\n")
            f.write("\t\t\t\"inherit-shown\": true,\n")
            f.write("\t\t\t\"z-layer\": 0,\n")
            f.write("\t\t\t\"tint\": [255, 255, 255, 255]\n")
            f.write("\t\t},\n")
            f.write("\t\t\"origin\": {\n")
            f.write("\t\t\t\"position\": {\n")
            f.write("\t\t\t\t\"scale\": [0, 0],\n")
            f.write("\t\t\t\t\"offset\": [0, 0]\n")
            f.write("\t\t\t},\n")
            f.write("\t\t\t\"size\": {\n")
            f.write("\t\t\t\t\"scale\": [0, 0],\n")
            f.write("\t\t\t\t\"offset\": [0, 0]\n")
            f.write("\t\t\t}\n")
            f.write("\t\t}\n")
    f.write("\t}\n")
    f.write("}")
