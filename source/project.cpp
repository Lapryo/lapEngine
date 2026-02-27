#include "project.hpp"
#include "json.hpp"
#include "eutil.hpp"

#include "elements.hpp"

#include <iostream>

using namespace lapCore;

rl::Color GetColorData(const nlohmann::json_abi_v3_12_0::json &dataJson, const std::string &key)
{
    if (dataJson.contains(key) && dataJson[key].is_array() && dataJson[key].size() == 4)
    {
        return (rl::Color){
            dataJson[key].at(0).get<unsigned char>(),
            dataJson[key].at(1).get<unsigned char>(),
            dataJson[key].at(2).get<unsigned char>(),
            dataJson[key].at(3).get<unsigned char>()};
    }
    else
        return rl::RayWhite;
}

rl::Vector4 GetVector4Data(const nlohmann::json_abi_v3_12_0::json &dataJson, const std::string &key)
{
    if (dataJson.contains(key) && dataJson[key].is_array() && dataJson[key].size() == 4)
    {
        return (rl::Vector4){
            dataJson[key].at(0).get<float>(),
            dataJson[key].at(1).get<float>(),
            dataJson[key].at(2).get<float>(),
            dataJson[key].at(3).get<float>()};
    }
    else
        return {0, 0, 0, 0};
}

Renderable GetRenderableData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Renderable renderable;

    if (dataJson.contains("renderable") && dataJson["renderable"].is_object())
    {
        const auto& renderableProps = dataJson["renderable"];

        renderable.isScreenSpace = renderableProps.value("isScreenSpace", false);
        renderable.visible = renderableProps.value("visible", true);
        renderable.zlayer = renderableProps.value("zlayer", 0);
        renderable.usesUIListVisiblity = renderableProps.value("usesUIListVisibility", false);

        renderable.tint = GetColorData(renderableProps, "tint");
    }
    else
        renderable = Renderable(0, false, true, rl::RayWhite, false);

    return renderable;
}

rl::Vector2 GetVector2Data(const nlohmann::json_abi_v3_12_0::json &dataJson, const std::string &key, unsigned int offset = 0)
{
    if (dataJson.contains(key) && dataJson[key].is_array() && dataJson[key].size() == 2)
    {
        return (rl::Vector2){
            dataJson[key].at(0 + offset).get<float>(),
            dataJson[key].at(1 + offset).get<float>()};
    }
    else
        return {0, 0};
}

FrameVector GetFrameVectorData(const nlohmann::json_abi_v3_12_0::json &dataJson, const std::string &key)
{
    FrameVector frameVector;

    if (dataJson.contains(key) && dataJson[key].is_array() && dataJson[key].size() == 4)
    {
        frameVector.scale = GetVector2Data(dataJson, key, 0);
        frameVector.offset = GetVector2Data(dataJson, key, 2);
    }
    else
        frameVector = FrameVector({0,0}, {0,0});

    return frameVector;
}

UIOrigin GetUIOriginData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    UIOrigin origin;

    if (dataJson.contains("origin") && dataJson["origin"].is_object())
    {
        const auto& originProps = dataJson["origin"];

        origin.position = GetFrameVectorData(originProps, "position");
        origin.size = GetFrameVectorData(originProps, "size");
    }
    else
        origin = UIOrigin(FrameVector({0,0}, {0,0}), FrameVector({0,0}, {0,0}));

    return origin;
}

Frame GetFrameData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    return Frame(GetRenderableData(dataJson), GetUIOriginData(dataJson));
}

Origin2D GetOrigin2DData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Origin2D origin2D;

    if (dataJson.contains("origin-2d") && dataJson["origin-2d"].is_object())
    {
        const auto& origin2DProps = dataJson["origin-2d"];

        origin2D.position = GetVector2Data(origin2DProps, "position");
        origin2D.scale = GetVector2Data(origin2DProps, "scale");
    }
    else
        origin2D = Origin2D{ {0,0}, {1,1} };

    return origin2D;
}

Physics2D GetPhysics2DData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Physics2D physics2D;

    if (dataJson.contains("physics-2d") && dataJson["physics-2d"].is_object())
    {
        const auto& physics2DProps = dataJson["physics-2d"];

        physics2D.velocity = GetVector2Data(physics2DProps, "velocity");
        physics2D.gravity = GetVector2Data(physics2DProps, "gravity");
    }
    else
        physics2D = Physics2D{ {0,0}, {0,0} };

    return physics2D;
}

Rotation2D GetRotation2DData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Rotation2D rotation2D;

    if (dataJson.contains("rotation-2d") && dataJson["rotation-2d"].is_object())
    {
        const auto& rotation2DProps = dataJson["rotation-2d"];

        rotation2D.anchor = GetVector2Data(rotation2DProps, "anchor");
        rotation2D.rotation = rotation2DProps.value("rotation", 0.0f);
    }
    else
        rotation2D = Rotation2D{ {0,0}, 0.0f };

    return rotation2D;
}

Sprite GetSpriteData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Sprite sprite;

    if (dataJson.contains("sprite") && dataJson["sprite"].is_object())
    {
        const auto& spriteProps = dataJson["sprite"];

        sprite.renderable = GetRenderableData(spriteProps);
        sprite.textureName = spriteProps.value("texture-name", "");
    }
    else
        sprite = Sprite(Renderable(0, false, true, rl::RayWhite, false), "");

    return sprite;
}

UIList GetUIListData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    UIList uiList;

    if (dataJson.contains("ui-list") && dataJson["ui-list"].is_object())
    {
        const auto& uiListProps = dataJson["ui-list"];

        uiList.scrollSize = GetFrameVectorData(uiListProps, "scroll-size");
        uiList.displaySize = GetFrameVectorData(uiListProps, "display-size");
        uiList.hScrollRight = uiListProps.value("h-scroll-right", true);
        uiList.vScrollBottom = uiListProps.value("v-scroll-bottom", true);
        uiList.maskOutsideContent = uiListProps.value("mask-outside-content", true);
        uiList.scrollOffset = uiListProps.value("scroll-offset", 0.0f);
        uiList.scrollSpeed = uiListProps.value("scroll-speed", 20.0f);

        std::string directionStr = uiListProps.value("direction", "vertical");
        if (directionStr == "horizontal")
            uiList.direction = Axis2D::HORIZONTAL;
        else
            uiList.direction = Axis2D::VERTICAL;
    }
    else
        uiList = UIList(FrameVector({0,0}, {0,0}), FrameVector({0,0}, {0,0}), true, true, true, 0.0f, 20.0f, Axis2D::VERTICAL);

    return uiList;
}

Image GetImageData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Image image;

    if (dataJson.contains("image") && dataJson["image"].is_object())
    {
        const auto& imageProps = dataJson["image"];

        image.sprite = GetSpriteData(imageProps);
        image.origin = GetUIOriginData(imageProps);
    }
    else
        image = Image(Sprite(Renderable(0, false, true, rl::RayWhite, false), ""), UIOrigin(FrameVector({0,0}, {0,0}), FrameVector({0,0}, {0,0})));

    return image;
}

Alignment GetAlignmentData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Alignment alignment;

    if (dataJson.contains("alignment") && dataJson["alignment"].is_object())
    {
        const auto& alignmentProps = dataJson["alignment"];

        std::string hAlignStr = alignmentProps.value("horizontal", "left");
        if (hAlignStr == "left")
            alignment.horizontal = HorizontalAlignment::LEFT;
        else if (hAlignStr == "middle")
            alignment.horizontal = HorizontalAlignment::MIDDLE;
        else
            alignment.horizontal = HorizontalAlignment::RIGHT;

        std::string vAlignStr = alignmentProps.value("vertical", "top");
        if (vAlignStr == "top")
            alignment.vertical = VerticalAlignment::TOP;
        else if (vAlignStr == "middle")
            alignment.vertical = VerticalAlignment::MIDDLE;
        else
            alignment.vertical = VerticalAlignment::BOTTOM;
    }
    else
        alignment = Alignment(HorizontalAlignment::LEFT, VerticalAlignment::TOP);

    return alignment;
}

Padding GetPaddingData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Padding padding;

    if (dataJson.contains("padding") && dataJson["padding"].is_array() && dataJson["padding"].size() == 4)
    {
        padding.top = dataJson["padding"].at(0).get<float>();
        padding.bottom = dataJson["padding"].at(1).get<float>();
        padding.left = dataJson["padding"].at(2).get<float>();
        padding.right = dataJson["padding"].at(3).get<float>();
    }
    else
        padding = Padding(0.0f, 0.0f, 0.0f, 0.0f);

    return padding;
}

TextLabel GetTextLabelData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    TextLabel textLabel;

    if (dataJson.contains("text-label") && dataJson["text-label"].is_object())
    {
        const auto& textLabelProps = dataJson["text-label"];

        textLabel.frame = GetFrameData(textLabelProps);
        textLabel.fontSize = textLabelProps.value("font-size", 16);
        textLabel.textAlignment = GetAlignmentData(textLabelProps);
        textLabel.textBounds = GetFrameVectorData(textLabelProps, "text-bounds");
        textLabel.textPadding = GetPaddingData(textLabelProps);
        textLabel.text = textLabelProps.value("text", "");
    }
    else
        textLabel = TextLabel(Frame(Renderable(0, true, true, rl::RayWhite, false), UIOrigin(FrameVector({0, 0}, {0, 0}), FrameVector({0, 0}, {0, 0}))), "", 0.0f, Alignment(HorizontalAlignment::LEFT, VerticalAlignment::TOP), FrameVector({0,0}, {0,0}), Padding(0.0f, 0.0f, 0.0f, 0.0f));

    return textLabel;
}

EventBus GetEventBusData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    EventBus eventBus;

    if (dataJson.contains("event-bus") && dataJson["event-bus"].is_object())
    {
        const auto& eventBusProps = dataJson["event-bus"];

        if (eventBusProps.contains("events") && eventBusProps["events"].is_object())
            for (auto& [name, event] : eventBusProps["events"].items())
                eventBus.events[name] = event.get<std::string>();
    }

    return eventBus;
}

UIButton GetUIButtonData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    UIButton uiButton;

    if (dataJson.contains("ui-button") && dataJson["ui-button"].is_object())
    {
        const auto& uiButtonProps = dataJson["ui-button"];

        uiButton.events = GetEventBusData(uiButtonProps);
        uiButton.bounds = GetUIOriginData(uiButtonProps);
        uiButton.active = uiButtonProps.value("active", true);
        uiButton.usesListVisibility = uiButtonProps.value("uses-uilist-visibility", false);
    }
    else
        uiButton = UIButton(EventBus(), UIOrigin(FrameVector({0,0}, {0,0}), FrameVector({0,0}, {0,0})), true, false);

    return uiButton;
}

Attribute<std::any> GetAttributeData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Attribute<std::any> attribute;

    if (dataJson.contains("attribute") && dataJson["attribute"].is_object())
    {
        const auto& attributeProps = dataJson["attribute"];

        attribute.name = attributeProps.value("name", "");

        if (attributeProps.contains("value"))
            attribute.value = attributeProps["value"];
        else
            attribute.value = std::any{};
    }
    else
        return Attribute<std::any>("", false);

    return attribute;
}

Cam2D GetCam2DData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Cam2D cam2D;

    if (dataJson.contains("cam-2d") && dataJson["cam-2d"].is_object())
    {
        const auto& cam2DProps = dataJson["cam-2d"];

        cam2D.camera.offset = GetVector2Data(cam2DProps, "offset");
        cam2D.camera.target = GetVector2Data(cam2DProps, "target");
        cam2D.camera.rotation = cam2DProps.value("rotation", 0.0f);
        cam2D.camera.zoom = cam2DProps.value("zoom", 1.0f);
        
        if (cam2DProps.contains("exclude") && cam2DProps["exclude"].is_array())
            for (const auto& entityJson : cam2DProps["exclude"])
                cam2D.exclude.push_back(entityJson.get<std::string>());
        else
            cam2D.exclude = {};
    }
    else
        cam2D = Cam2D{ {{0,0}, {0,0}, 0.0f, 1.0f}, {} };

    return cam2D;
}

Script GetScriptData(const nlohmann::json_abi_v3_12_0::json &dataJson)
{
    Script script;

    if (dataJson.contains("script") && dataJson["script"].is_object())
    {
        const auto& scriptProps = dataJson["script"];

        script.onCreateFunction = scriptProps.value("create-function", "");
        script.onUpdateFunction = scriptProps.value("update-function", "");
        script.onDestroyFunction = scriptProps.value("destroy-function", "");

        script.active = scriptProps.value("active", true);
    }
    else
        script = Script{ "", "", "" };

    return script;
}

std::variant<
    std::monostate,
    Origin2D,
    Physics2D,
    Rotation2D,
    Frame,
    UIList,
    Sprite,
    Image,
    TextLabel,
    EventBus,
    UIButton,
    Cam2D,
    Attribute<std::any>,
    Script
> GetElementData(const nlohmann::json_abi_v3_12_0::json &elementJson)
{
    std::string elementType = elementJson.value("type", "");
    if (elementJson.contains("data") && elementJson["data"].is_object())
    {
        const auto& data = elementJson["data"];

        if (elementType == "frame") // Returns a Frame struct
            return GetFrameData(data);
        else if (elementType == "origin-2d") // Returns a UIOrigin struct
            return GetOrigin2DData(data);
        else if (elementType == "physics-2d") // Returns a Physics2D struct
            return GetPhysics2DData(data);
        else if (elementType == "rotation-2d") // Returns a Rotation2D struct
            return GetRotation2DData(data);
        else if (elementType == "sprite") // Returns a Sprite struct
            return GetSpriteData(data);
        else if (elementType == "ui-list") // Returns a UIList struct
            return GetUIListData(data);
        else if (elementType == "image") // Returns an Image struct
            return GetImageData(data);
        else if (elementType == "text-label") // Returns a TextLabel struct
            return GetTextLabelData(data);
        else if (elementType == "event-bus") // Returns an EventBus struct
            return GetEventBusData(data);
        else if (elementType == "ui-button") // Returns a UIButton struct
            return GetUIButtonData(data);
        else if (elementType == "attribute") // Returns an Attribute<T> struct
            return GetAttributeData(data);
        else if (elementType == "cam-2d") // Returns a Cam2D struct
            return GetCam2DData(data);
        else if (elementType == "script") // Returns a Script struct
            return GetScriptData(data);
        else
            std::cout << "[WARNING] Unknown element type: " << elementType << '\n';
    }
    else
        std::cout << (elementJson.contains("data") ? "[NOTICE] Element did not contain any data\n" : "[WARNING] Element data was not an object\n");

    return std::monostate{};
}

// Gathers data from an element JSON object, which should contain a type string and data array, returns a ProjectElementData struct
ProjectElementData GetElement(const nlohmann::json_abi_v3_12_0::json &elementJson)
{
    std::string elementType = elementJson.value("type", "");
    std::cout << "Loading element: " << elementType << '\n';

    ProjectElementData elementData;
    elementData.type = elementType;
    
    elementData.data = GetElementData(elementJson);
    return elementData;
}

// Loads elements from either object JSONs or external files, returns a vector of ProjectElementData structs
std::vector<ProjectElementData> GetElements(const nlohmann::json_abi_v3_12_0::json &objectJson, const std::string &sceneName)
{
    std::vector<ProjectElementData> elements; // Vector to hold the elements

    if (objectJson.contains("elements") && objectJson["elements"].is_array() && !objectJson["elements"].empty())
    {
        for (const auto &elementJson : objectJson["elements"])
        {
            if (elementJson.is_string())
            {
                std::string elementFilePath = elementJson.get<std::string>();
                std::cout << "Loading element from file: " << elementFilePath << '\n';

                std::string prefixFilePath = "assets/project/scenes/" + sceneName + "/instances/" + objectJson.value("name", "Unnamed Object") + "/elements/";
                nlohmann::json elementJsonFromFile = ReadFileToJsonObject(prefixFilePath + elementFilePath);
                elements.push_back(GetElement(elementJsonFromFile));
            }
            else if (elementJson.is_object())
                elements.push_back(GetElement(elementJson));
            else
                std::cout << "[WARNING] Element entry was neither a string nor an object, skipping...\n";
        }
    }
    else
        if (!objectJson.contains("elements"))
            std::cout << "[WARNING] Object did not contain an elements property\n";
        else if (!objectJson["elements"].is_array())
            std::cout << "[WARNING] Object elements property was not an array\n";
        else if (objectJson["elements"].empty())
            std::cout << "[NOTICE] Object had no elements in the elements property array\n";
        else
            std::cout << "[WARNING] Something else went wrong with loading elements\n";
    
    return elements;
}

// Gathers data from an object JSON object, which should contain name string, parent string, child-index int, and components array, returns a ProjectObjectData struct
ProjectObjectData GetObject(const nlohmann::json_abi_v3_12_0::json &objectJson, const std::string &sceneName)
{
    std::string objectName = objectJson.value("name", "");
    std::string objectParent = objectJson.value("parent", "");
    int objectChildIndex = objectJson.value("child-index", -1);
    std::cout << "Loading object: " << objectName << "\nChild index: " << objectChildIndex << "\nParent: " << objectParent << '\n';

    ProjectObjectData objectData;
    objectData.name = objectName;
    objectData.parent = objectParent;
    objectData.child_index = objectChildIndex;

    objectData.elements = GetElements(objectJson, sceneName);

    return objectData;
}

std::vector<ProjectObjectData> GetInstances(const nlohmann::json_abi_v3_12_0::json &sceneJson)
{
    std::vector<ProjectObjectData> instances; // Vector to hold the instances

    if (sceneJson.contains("instances") && sceneJson["instances"].is_object() && !sceneJson["instances"].empty())
    {
        if (sceneJson["instances"].contains("objects") && sceneJson["instances"]["objects"].is_array() && !sceneJson["instances"]["objects"].empty())
        {
            for (const auto &objectJson : sceneJson["instances"]["objects"])
            {
                if (objectJson.is_string())
                {
                    std::string objectFilePath = objectJson.get<std::string>();
                    std::cout << "Loading instance from file: " << objectFilePath << '\n';

                    nlohmann::json objectJsonFromFile = ReadFileToJsonObject("assets/project/scenes/" + sceneJson.value("name", "") + "/objects/" + objectFilePath);
                    instances.push_back(GetObject(objectJsonFromFile, sceneJson.value("name", "")));
                }
                else if (objectJson.is_object())
                    instances.push_back(GetObject(objectJson, sceneJson.value("name", "")));
                else
                    std::cout << "[WARNING] Object entry was neither a string (file path) nor an object, skipping...\n";
            }
        }
        else
            if (!sceneJson["instances"].contains("objects"))
                std::cout << "[WARNING] Instances did not contain an objects property\n";
            else if (!sceneJson["instances"]["objects"].is_array())
                std::cout << "[WARNING] Instances objects property was not an array\n";
            else if (sceneJson["instances"]["objects"].empty())
                std::cout << "[NOTICE] Instances had no objects in the objects property array\n";
            else
                std::cout << "[WARNING] Something went wrong with loading instanced objects\n";

        if (sceneJson["instances"].contains("prefabs") && sceneJson["instances"]["prefabs"].is_array() && !sceneJson["instances"]["prefabs"].empty())
        {
            for (const auto &prefabJson : sceneJson["instances"]["prefabs"])
            {
                if (prefabJson.is_string())
                {
                    std::string prefabFilePath = prefabJson.get<std::string>();
                    std::cout << "Loading instance from prefab file: " << prefabFilePath << '\n';

                    nlohmann::json prefabJsonFromFile = ReadFileToJsonObject("assets/project/prefabs/" + prefabFilePath);
                    instances.push_back(GetObject(prefabJsonFromFile, sceneJson.value("name", "")));
                }
                else if (prefabJson.is_object())
                    instances.push_back(GetObject(prefabJson, sceneJson.value("name", "")));
                else
                    std::cout << "[WARNING] Prefab entry was neither a string (file path) nor an object, skipping...\n";
            }
        }
        else
            if (!sceneJson["instances"].contains("prefabs"))
                std::cout << "[WARNING] Instances did not contain a prefabs property\n";
            else if (!sceneJson["instances"]["prefabs"].is_array())
                std::cout << "[WARNING] Instances prefabs property was not an array\n";
            else if (sceneJson["instances"]["prefabs"].empty())
                std::cout << "[NOTICE] Instances had no prefabs in the prefabs property array\n";
            else
                std::cout << "[WARNING] Something went wrong with loading instanced prefabs\n";
    }
    else
        if (!sceneJson.contains("instances"))
            std::cout << "[WARNING] Scene did not contain an instances property\n";
        else if (!sceneJson["instances"].is_object())
            std::cout << "[WARNING] Scene instances property was not an object\n";
        else if (sceneJson["instances"].empty())
            std::cout << "[NOTICE] Scene had no properties in the instances object\n";
        else
            std::cout << "[WARNING] Something went wrong with loading instances\n";

    return instances;
}

std::vector<ProjectObjectData> GetPrefabs(const nlohmann::json_abi_v3_12_0::json &projectJson)
{
    std::vector<ProjectObjectData> prefabs; // Vector to hold the prefabs

    if (projectJson.contains("prefabs") && projectJson["prefabs"].is_array() && !projectJson["prefabs"].empty())
    {
        for (const auto &prefabJson : projectJson["prefabs"])
        {
            if (prefabJson.is_string())
            {
                std::string prefabFilePath = prefabJson.get<std::string>();
                std::cout << "Loading prefab from file: " << prefabFilePath << '\n';

                nlohmann::json prefabJsonFromFile = ReadFileToJsonObject("assets/project/prefabs/" + prefabFilePath);
                prefabs.push_back(GetObject(prefabJsonFromFile, "prefabs"));
            }
            else if (prefabJson.is_object())
                prefabs.push_back(GetObject(prefabJson, "prefabs"));
            else
                std::cout << "[WARNING] Prefab entry was neither a string (file path) nor an object, skipping...\n";
        }
    }
    else
        if (!projectJson.contains("prefabs"))
            std::cout << "[WARNING] Project did not contain an prefabs property\n";
        else if (!projectJson["prefabs"].is_array())
            std::cout << "[WARNING] Project prefabs property was not an array\n";
        else if (projectJson["prefabs"].empty())
            std::cout << "[NOTICE] Project had no prefabs in the prefabs property array\n";
        else
            std::cout << "[WARNING] Something else went wrong with loading prefabs\n";

    return prefabs;
}

// Gathers data from a system JSON object, which should contain a type string and order u-int, returns a ProjectSystemData struct
ProjectSystemData GetSystem(const nlohmann::json_abi_v3_12_0::json &systemJson)
{
    // Gather the system values
    std::string systemType = systemJson.value("type", "");
    int systemOrder = systemJson.value("order", -1);
    std::cout << "Loading " << systemType << " system\nSystem order: " << systemOrder << '\n';

    // Assign them to a ProjectSystemData struct and return it
    ProjectSystemData systemData;
    systemData.type = systemJson.value("type", "");
    systemData.order = systemJson.value("order", -1);

    return systemData;
}

// Loads systems from either scene JSONs or external files, returns a vector of ProjectSystemData structs
std::vector<ProjectSystemData> GetSystems(const nlohmann::json_abi_v3_12_0::json &sceneJson)
{
    std::vector<ProjectSystemData> systems; // Vector to hold the systems

    if (sceneJson.contains("systems") && sceneJson["systems"].is_array() && !sceneJson["systems"].empty()) // Similar to GetAssets(), check if there is an array for systems, loop through, gather data
    {
        for (const auto &systemJson : sceneJson["systems"])
        {
            if (systemJson.is_string())
            {
                std::string systemFilePath = systemJson.get<std::string>();
                std::cout << "Loading system from file: " << systemFilePath << '\n';

                nlohmann::json systemJsonFromFile = ReadFileToJsonObject("assets/project/scenes/" + sceneJson.value("name", "Unnamed Scene") + "/systems/" + systemFilePath);

                systems.push_back(GetSystem(systemJsonFromFile));
            }
            else if (systemJson.is_object())
                systems.push_back(GetSystem(systemJson));
            else
                std::cout << "[WARNING] System entry was neither a string nor an object, skipping...\n";
        }
    }
    else
        if (!sceneJson.contains("systems"))
            std::cout << "[WARNING] Scene did not contain a systems property\n";
        else if (!sceneJson["systems"].is_array())
            std::cout << "[WARNING] Scene systems property was not an array\n";
        else if (sceneJson["systems"].empty())
            std::cout << "[NOTICE] Scene had no systems in the systems property array\n";
        else
            std::cout << "[WARNING] Something else went wrong with loading systems\n";

    return systems; // Return the gathered systems
}

// Gathers data from a scene JSON object, which should contain objects, prefabs, and systems arrays, returns a ProjectSceneData struct
ProjectSceneData GetScene(const nlohmann::json_abi_v3_12_0::json &sceneJson, std::string sceneName = "")
{
    ProjectSceneData sceneData;

    // Gather objects, prefabs, and systems into a ProjectSceneData struct and return it
    sceneData.name = sceneJson.value("name", "Unnamed Scene");
    sceneData.instances = GetInstances(sceneJson);
    sceneData.systems = GetSystems(sceneJson);

    std::cout << "Finished gathering scene data from: " << sceneData.name << "\n";

    return sceneData;
}

// Loads scenes from either project JSONs or external files, returns a vector of ProjectSceneData structs gathered from GetScene()
std::vector<ProjectSceneData> GetScenes(const nlohmann::json_abi_v3_12_0::json &projectJson)
{
    std::vector<ProjectSceneData> scenes; // Vector to hold the scenes

    if (projectJson.contains("scenes") && projectJson["scenes"].is_array() && !projectJson["scenes"].empty()) // Similar to GetAssets(), check if there is an array for scenes, loop through, gather data
    {
        for (const auto &sceneJson : projectJson["scenes"])
        {
            if (sceneJson.is_string())
            {
                std::string sceneFilePath = sceneJson.get<std::string>();
                std::cout << "Loading scene from file: " << sceneFilePath << '\n';

                // To get the file name, we remove the .json / .(anything) extension from the path
                std::string sceneFileName = sceneFilePath;
                size_t lastSlash = sceneFileName.find_last_of("/\\");
                if (lastSlash != std::string::npos)
                    sceneFileName = sceneFileName.substr(lastSlash + 1);
                size_t lastDot = sceneFileName.find_last_of('.');
                if (lastDot != std::string::npos)
                    sceneFileName = sceneFileName.substr(0, lastDot);

                nlohmann::json sceneJsonFromFile = ReadFileToJsonObject("assets/project/scenes/" + sceneFileName + '/' + sceneFilePath);
                scenes.push_back(GetScene(sceneJsonFromFile));
            }
            else if (sceneJson.is_object())
                scenes.push_back(GetScene(sceneJson, sceneJson.value("name", "Unnamed Scene"))); // Pass the scene name for logging
            else
                std::cout << "[WARNING] Scene entry was neither a string nor an object, skipping...\n";
        }
    }
    else // If not, log a message defining the error and continue forward
        if (!projectJson.contains("scenes"))
            std::cout << "[WARNING] Project did not contain an scenes property\n";
        else if (!projectJson["scenes"].is_array())
            std::cout << "[WARNING] Scenes property was not an array\n";
        else if (projectJson["scenes"].empty())
            std::cout << "[NOTICE] Project had no scenes in the scenes property array\n";
        else
            std::cout << "[WARNING] Something else went wrong with loading scenes\n";

    return scenes; // Return the gathered scenes
}

// Gathers data from an asset JSON object, which should contain name, type, and path strings, returns a ProjectAssetData struct
ProjectAssetData GetAsset(const nlohmann::json_abi_v3_12_0::json &assetJson)
{
    // Gather the values
    const auto &assetName = assetJson.value("name", "");
    const auto &assetType = assetJson.value("type", "");
    const auto &assetPath = assetJson.value("path", "");
    std::cout << "Loading " << assetType << " asset: " << assetName << "\nAsset path: " << assetPath << '\n';

    // Assign them to a ProjectAssetData struct and return it
    ProjectAssetData assetData;
    assetData.name = assetName;
    assetData.type = assetType;
    assetData.path = assetPath;

    return assetData;
}

// Loads assets from either project JSONs or external files, returns a vector of ProjectAssetData structs gathered from GetAsset()
std::vector<ProjectAssetData> GetAssets(const nlohmann::json_abi_v3_12_0::json &projectJson)
{
    std::vector<ProjectAssetData> assets; // Vector to hold the assets

    if (projectJson.contains("assets") && projectJson["assets"].is_array() && !projectJson["assets"].empty()) // Check if there is an array for assets
    {
        for (const auto &assetJson : projectJson["assets"]) // Loop through each asset
        {
            if (assetJson.is_string()) // If the asset is a string and not a JSON object, that means it's an external file, and we must load it from there
            {
                std::string assetFilePath = assetJson.get<std::string>(); // Get the file path string, which should be relative to the assets/project/assets/ directory
                std::cout << "Loading asset from file: " << assetFilePath << '\n';

                nlohmann::json assetJsonFromFile = ReadFileToJsonObject("assets/project/assets/" + assetFilePath); // Read the file into a string, and parse it into a JSON object
                assets.push_back(GetAsset(assetJsonFromFile)); // Gather the asset data and add it to the assets vector
            }
            else if (assetJson.is_object()) // If it's a normal JSON object, we can just gather the data directly
                assets.push_back(GetAsset(assetJson));
            else // If it's neither, log a warning and skip it
                std::cout << "[WARNING] Asset entry was neither a string nor an object, skipping...\n";
        }
    }
    else // If not, log a message and continue forward
        if (!projectJson.contains("assets"))
            std::cout << "[WARNING] Project did not contain an assets property\n";
        else if (!projectJson["assets"].is_array())
            std::cout << "[WARNING] Project assets property was not an array\n";
        else if (projectJson["assets"].empty())
            std::cout << "[NOTICE] Project had no assets in the assets property array\n";
        else
            std::cout << "[WARNING] Something else went wrong with loading assets\n";

    return assets; // Return the gathered assets
}

size_t GetMainScene(const nlohmann::json_abi_v3_12_0::json &projectJson)
{
    if (projectJson.contains("scenes") && projectJson["scenes"].is_array() && !projectJson["scenes"].empty())
    {
        for (size_t i = 0; i < projectJson["scenes"].size(); ++i)
        {
            const auto &sceneEntry = projectJson["scenes"].at(i);
            std::string sceneName;

            if (sceneEntry.is_string())
            {
                std::string sceneFilePath = sceneEntry.get<std::string>();
                size_t lastSlash = sceneFilePath.find_last_of("/\\");
                size_t lastDot = sceneFilePath.find_last_of('.');
                if (lastDot != std::string::npos)
                    sceneName = sceneFilePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
                else
                    sceneName = sceneFilePath.substr(lastSlash + 1);
            }
            else if (sceneEntry.is_object())
                sceneName = sceneEntry.value("name", "Unnamed Scene");
            else
            {
                std::cout << "[WARNING] Scene entry was neither a string nor an object, skipping...\n";
                continue;
            }

            if (sceneName == "main")
            {
                std::cout << "Main scene found: " << sceneName << " at index " << i << '\n';
                return i;
            }
        }
    }
    else
        std::cout << "[WARNING] Project did not contain any scenes to search for the main scene\n";
    return -1; // Return 0 if no main scene is found
}

// Unpacks a project file from a JSON file path into a project object, returns the Project struct
Project lapCore::UnpackProject(const std::string projectJsonString)
{
    nlohmann::json projectJson = nlohmann::json::parse(projectJsonString); // Parse the JSON string into a JSON object
    std::string projectName = projectJson.value("name", "Unnamed Project");
    std::cout << "[PROJECT] Unpacking project: " << projectName << '\n';

    std::string projectVersion = projectJson.value("version", "0.0");
    std::cout << "[PROJECT] Version: " << projectVersion << '\n';

    Project project; // Create the project object, asign its values
    project.name = projectName;
    project.version = projectVersion;

    project.main_scene_index = GetMainScene(projectJson);

    project.assets = GetAssets(projectJson); // Gather the data from the JSON
    project.scenes = GetScenes(projectJson);
    project.prefabs = GetPrefabs(projectJson);

    return project;
}