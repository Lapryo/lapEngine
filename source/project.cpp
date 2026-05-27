#include "project.hpp"
#include <nlohmann/json.hpp>
#include "eutil.hpp"

#include "elements.hpp"

#include "reflection.hpp"

#include <iostream>

using namespace lapCore;

// Gathers data from an element JSON object, which should contain a type string and data array, returns a ProjectElementData struct
std::unique_ptr<IProjectElementData> GetElement(const nlohmann::json_abi_v3_12_0::json &elementJson)
{
    std::string elementType = elementJson.value("type", "");
    dbgln("Loading element: " + elementType, LogType::INFO);

    auto entry = Reflection::TryGet(elementType);
    if (!entry)
    {
        dbgln("No type registry entry found for element type: " + elementType, LogType::WARNING);
        return nullptr;
    }

    auto element = entry->create_project_data(elementJson.at("data"), elementType);
    
    std::string elementSynced = elementJson.value("synced", "none");
    if (elementSynced == "update-on-scene")
        element->synced = SyncProjectRuntimeMode::UPDATE_ON_SCENE;
    else if (elementSynced == "constant")
        element->synced = SyncProjectRuntimeMode::CONSTANT;
    else
        element->synced = SyncProjectRuntimeMode::NONE;
    
    return element;
}

// Loads elements from either object JSONs or external files, returns a vector of ProjectElementData structs
std::vector<std::unique_ptr<IProjectElementData>> GetElements(const nlohmann::json_abi_v3_12_0::json &objectJson, const std::string &sceneName)
{
    std::vector<std::unique_ptr<IProjectElementData>> elements; // Vector to hold the elements

    if (objectJson.contains("elements") && objectJson["elements"].is_array() && !objectJson["elements"].empty())
    {
        for (const auto &elementJson : objectJson["elements"])
        {
            if (elementJson.is_string())
            {
                std::string elementFilePath = elementJson.get<std::string>();
                dbgln("Loading element from file: " + elementFilePath, LogType::INFO);

                std::string prefixFilePath = "assets/project/scenes/" + sceneName + "/objects/" + objectJson.value("name", "Unnamed Object") + "/elements/";
                nlohmann::json elementJsonFromFile = ReadFileToJsonObject(prefixFilePath + elementFilePath);
                elements.push_back(GetElement(elementJsonFromFile));
            }
            else if (elementJson.is_object())
                elements.push_back(GetElement(elementJson));
            else
                dbgln("Element entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
        }
    }
    else
        if (!objectJson.contains("elements"))
            dbgln("Object did not contain an elements property", LogType::WARNING);
        else if (!objectJson["elements"].is_array())
            dbgln("Object elements property was not an array", LogType::WARNING);
        else if (objectJson["elements"].empty())
            dbgln("Object had no elements in the elements property array", LogType::WARNING);
        else
            dbgln("Something else went wrong with loading elements", LogType::WARNING);

    return elements;
}

// Gathers data from an object JSON object, which should contain name string, parent string, child-index int, and components array, returns a ProjectObjectData struct
ProjectObjectData GetObject(const nlohmann::json_abi_v3_12_0::json &objectJson, const std::string &sceneName)
{
    std::string objectName = objectJson.value("name", "");
    std::string objectParent = objectJson.value("parent", "");
    int objectChildIndex = objectJson.value("child-index", -1);
    dbgln("Loading object: " + objectName, LogType::INFO);
    dbgln("Child index: " + std::to_string(objectChildIndex), LogType::INFO);
    dbgln("Parent: " + objectParent, LogType::INFO);

    ProjectObjectData objectData;
    objectData.name = objectName;
    objectData.parent = objectParent;
    objectData.child_index = objectChildIndex;

    objectData.elements = GetElements(objectJson, sceneName);

    return objectData;
}

std::vector<ProjectObjectData> GetObjectInstances(const nlohmann::json_abi_v3_12_0::json &sceneJson)
{
    std::vector<ProjectObjectData> objectInstances;

    if (sceneJson.contains("instances") && sceneJson["instances"].is_object() && !sceneJson["instances"].empty())
    {
        if (sceneJson["instances"].contains("objects") && sceneJson["instances"]["objects"].is_array() && !sceneJson["instances"]["objects"].empty())
        {
            for (const auto &objectJson : sceneJson["instances"]["objects"])
            {
                if (objectJson.is_string())
                {
                    std::string objectFilePath = objectJson.get<std::string>();
                    dbgln("Loading instance from file: " + objectFilePath, LogType::INFO);

                    // To get the file name, we remove the .json / .(anything) extension from the path
                    std::string objectFileName = objectFilePath;
                    size_t lastSlash = objectFileName.find_last_of("/\\");
                    if (lastSlash != std::string::npos)
                        objectFileName = objectFileName.substr(lastSlash + 1);
                    size_t lastDot = objectFileName.find_last_of('.');
                    if (lastDot != std::string::npos)
                        objectFileName = objectFileName.substr(0, lastDot);

                    nlohmann::json objectJsonFromFile = ReadFileToJsonObject("assets/project/scenes/" + sceneJson.value("name", "") + "/objects/" + objectFileName + "/" + objectFilePath);
                    objectInstances.push_back(GetObject(objectJsonFromFile, sceneJson.value("name", "")));
                }
                else if (objectJson.is_object())
                    objectInstances.push_back(GetObject(objectJson, sceneJson.value("name", "")));
                else
                    dbgln("Object entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
            }
        }
        else
            if (!sceneJson["instances"].contains("objects"))
                dbgln("Instances did not contain an objects property", LogType::WARNING);
            else if (!sceneJson["instances"]["objects"].is_array())
                dbgln("Instances objects property was not an array", LogType::WARNING);
            else if (sceneJson["instances"]["objects"].empty())
                dbgln("Instances had no objects in the objects property array", LogType::WARNING);
            else
                dbgln("Something went wrong with loading instanced objects", LogType::WARNING);
    }
    else
        if (!sceneJson.contains("instances"))
            dbgln("Scene did not contain an instances property", LogType::WARNING);
        else if (!sceneJson["instances"].is_object())
            dbgln("Scene instances property was not an object", LogType::WARNING);
        else if (sceneJson["instances"].empty())
            dbgln("Scene had no instances in the instances property object", LogType::WARNING);
        else
            dbgln("Something went wrong with loading instances", LogType::WARNING);

    return objectInstances;
}

std::vector<ProjectObjectData> GetPrefabInstances(const nlohmann::json_abi_v3_12_0::json &sceneJson, std::vector<ProjectObjectData> prefabs)
{
    std::vector<ProjectObjectData> prefabInstances;

    if (sceneJson.contains("instances") && sceneJson["instances"].is_object() && !sceneJson["instances"].empty())
    {
        if (sceneJson["instances"].contains("prefabs") && sceneJson["instances"]["prefabs"].is_array() && !sceneJson["instances"]["prefabs"].empty())
        {
            for (const auto &prefabJson : sceneJson["instances"]["prefabs"])
            {
                // This will be a bit tricky, index 1 will be the prefab name, index 2 will be any modifications made to that prefab, index 3 will be the count of those instances
                for (int i = 0; i < prefabJson.at(2).get<unsigned int>(); i++)
                {
                    std::string prefabName = prefabJson.at(0).get<std::string>();
                    dbgln("Loading prefab instance: " + prefabName, LogType::INFO);

                    for (const auto &prefab : prefabs)
                    {
                        if (prefab.name == prefabName)
                        {
                            ProjectObjectData instanceData = prefab;

                            // Apply modifications from prefabJson.at(1) to instanceData here, if there are any
                            for (const auto &modificationJson : prefabJson.at(1))
                            {
                                std::string elementType = modificationJson.value("type", "");
                                dbgln("Applying modification to prefab instance: " + elementType, LogType::INFO);

                                bool elementFoundInPrefab = false;

                                for (auto &element : instanceData.elements)
                                {
                                    if (element.get()->GetTypeID() == Reflection::reverseLookup[elementType])
                                    {
                                        elementFoundInPrefab = true;

                                        auto entry = Reflection::TryGet(elementType);
                                        if (!entry)
                                        {
                                            dbgln("Unknown element type in prefab modification: " + elementType, LogType::WARNING);
                                            break;
                                        }


                                        auto newElement = entry->create_project_data(modificationJson.at("data"), elementType);

                                        // replace element in instanceData with this new modified element
                                        element = std::move(newElement);
                                        break;
                                    }
                                }

                                if (!elementFoundInPrefab)
                                {
                                    // add the modification
                                    auto entry = Reflection::TryGet(elementType);
                                    if (!entry)
                                    {
                                        dbgln("Unknown element type in prefab modification: " + elementType, LogType::WARNING);
                                        continue;
                                    }

                                    auto element = entry->create_project_data(modificationJson.at("data"), elementType);
                                    instanceData.elements.push_back(std::move(element));
                                    continue;
                                }
                            }

                            prefabInstances.push_back(instanceData);
                            break;
                        }
                    }
                }
            }
        }
        else
            if (!sceneJson["instances"].contains("prefabs"))
                dbgln("Instances did not contain a prefabs property", LogType::WARNING);
            else if (!sceneJson["instances"]["prefabs"].is_array())
                dbgln("Instances prefabs property was not an array", LogType::WARNING);
            else if (sceneJson["instances"]["prefabs"].empty())
                dbgln("Instances had no prefabs in the prefabs property array", LogType::WARNING);
            else
                dbgln("Something went wrong with loading instanced prefabs", LogType::WARNING);
    }
    else
        if (!sceneJson.contains("instances"))
            dbgln("Scene did not contain an instances property", LogType::WARNING);
        else if (!sceneJson["instances"].is_object())
            dbgln("Scene instances property was not an object", LogType::WARNING);
        else if (sceneJson["instances"].empty())
            dbgln("Scene had no instances in the instances property object", LogType::WARNING);
        else
            dbgln("Something went wrong with loading instances", LogType::WARNING);

    return prefabInstances;
}

SceneInstancesData GetInstances(const json &sceneJson, std::vector<ProjectObjectData> prefabs)
{
    SceneInstancesData instances;

    instances.objects = GetObjectInstances(sceneJson);
    instances.prefabs = GetPrefabInstances(sceneJson, prefabs);

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
                dbgln("Loading prefab instance from file: " + prefabFilePath, LogType::INFO);

                // To get the file name, we remove the .json / .(anything) extension from the path
                std::string prefabFileName = prefabFilePath;
                size_t lastSlash = prefabFileName.find_last_of("/\\");
                if (lastSlash != std::string::npos)
                    prefabFileName = prefabFileName.substr(lastSlash + 1);
                size_t lastDot = prefabFileName.find_last_of('.');
                if (lastDot != std::string::npos)
                    prefabFileName = prefabFileName.substr(0, lastDot);

                nlohmann::json prefabJsonFromFile = ReadFileToJsonObject("assets/project/prefabs/" + prefabFileName + "/" + prefabFilePath);
                prefabs.push_back(GetObject(prefabJsonFromFile, "prefabs"));
            }
            else if (prefabJson.is_object())
                prefabs.push_back(GetObject(prefabJson, "prefabs"));
            else
                dbgln("[WARNING] Prefab entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
        }
    }
    else
        if (!projectJson.contains("prefabs"))
            dbgln("Project did not contain an prefabs property", LogType::WARNING);
        else if (!projectJson["prefabs"].is_array())
            dbgln("Project prefabs property was not an array", LogType::WARNING);
        else if (projectJson["prefabs"].empty())
            dbgln("Project had no prefabs in the prefabs property array", LogType::WARNING);
        else
            dbgln("Something else went wrong with loading prefabs", LogType::WARNING);

    return prefabs;
}

// Gathers data from a system JSON object, which should contain a type string and order u-int, returns a ProjectSystemData struct
ProjectSystemData GetSystem(const nlohmann::json_abi_v3_12_0::json &systemJson)
{
    // Gather the system values
    std::string systemType = systemJson.value("type", "");
    int systemOrder = systemJson.value("order", -1);
    dbgln("Loading " + systemType + " system with order: " + std::to_string(systemOrder), LogType::INFO);

    // Assign them to a ProjectSystemData struct and return it
    ProjectSystemData systemData;
    systemData.type = systemType;
    systemData.order = systemOrder;

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
                dbgln("Loading system from file: " + systemFilePath, LogType::INFO);

                nlohmann::json systemJsonFromFile = ReadFileToJsonObject("assets/project/scenes/" + sceneJson.value("name", "Unnamed Scene") + "/systems/" + systemFilePath);

                systems.push_back(GetSystem(systemJsonFromFile));
            }
            else if (systemJson.is_object())
                systems.push_back(GetSystem(systemJson));
            else
                dbgln("System entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
        }
    }
    else
        if (!sceneJson.contains("systems"))
            dbgln("Scene did not contain a systems property", LogType::WARNING);
        else if (!sceneJson["systems"].is_array())
            dbgln("Scene systems property was not an array", LogType::WARNING);
        else if (sceneJson["systems"].empty())
            dbgln("Scene had no systems in the systems property array", LogType::WARNING);
        else
            dbgln("Something else went wrong with loading systems", LogType::WARNING);

    return systems; // Return the gathered systems
}

// Gathers data from a scene JSON object, which should contain objects, prefabs, and systems arrays, returns a ProjectSceneData struct
ProjectSceneData GetScene(const nlohmann::json_abi_v3_12_0::json &sceneJson, std::vector<ProjectObjectData> prefabs = {})
{
    ProjectSceneData sceneData;

    // Gather objects, prefabs, and systems into a ProjectSceneData struct and return it
    sceneData.name = sceneJson.value("name", "Unnamed Scene");
    sceneData.instances = GetInstances(sceneJson, prefabs);
    sceneData.systems = GetSystems(sceneJson);

    dbgln("Finished gathering scene data from: " + sceneData.name, LogType::INFO);

    return sceneData;
}

// Loads scenes from either project JSONs or external files, returns a vector of ProjectSceneData structs gathered from GetScene()
std::vector<ProjectSceneData> GetScenes(const nlohmann::json_abi_v3_12_0::json &projectJson, std::vector<ProjectObjectData> prefabs)
{
    std::vector<ProjectSceneData> scenes; // Vector to hold the scenes

    if (projectJson.contains("scenes") && projectJson["scenes"].is_array() && !projectJson["scenes"].empty()) // Similar to GetAssets(), check if there is an array for scenes, loop through, gather data
    {
        for (const auto &sceneJson : projectJson["scenes"])
        {
            if (sceneJson.is_string())
            {
                std::string sceneFilePath = sceneJson.get<std::string>();
                dbgln("Loading scene from file: " + sceneFilePath, LogType::INFO);

                // To get the file name, we remove the .json / .(anything) extension from the path
                std::string sceneFileName = sceneFilePath;
                size_t lastSlash = sceneFileName.find_last_of("/\\");
                if (lastSlash != std::string::npos)
                    sceneFileName = sceneFileName.substr(lastSlash + 1);
                size_t lastDot = sceneFileName.find_last_of('.');
                if (lastDot != std::string::npos)
                    sceneFileName = sceneFileName.substr(0, lastDot);

                nlohmann::json sceneJsonFromFile = ReadFileToJsonObject("assets/project/scenes/" + sceneFileName + '/' + sceneFilePath);
                scenes.push_back(GetScene(sceneJsonFromFile, prefabs));
            }
            else if (sceneJson.is_object())
                scenes.push_back(GetScene(sceneJson, prefabs));
            else
                dbgln("Scene entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
        }
    }
    else // If not, log a message defining the error and continue forward
        if (!projectJson.contains("scenes"))
            dbgln("Project did not contain an scenes property", LogType::WARNING);
        else if (!projectJson["scenes"].is_array())
            dbgln("Scenes property was not an array", LogType::WARNING);
        else if (projectJson["scenes"].empty())
            dbgln("Project had no scenes in the scenes property array", LogType::WARNING);
        else
            dbgln("Something else went wrong with loading scenes", LogType::WARNING);

    return scenes; // Return the gathered scenes
}

std::unordered_map<std::string, std::string> GetAssetData(const nlohmann::json &assetJson)
{
    std::unordered_map<std::string, std::string> assetData; // Vector to hold the asset data

    if (assetJson.contains("data") && assetJson["data"].is_object())
    {
        for (auto it = assetJson["data"].begin(); it != assetJson["data"].end(); ++it)
        {
            auto key = it.key();
            if (it.value().is_string())
            {
                assetData[key] = it.value().get<std::string>();
            }
            else if (it.value().is_boolean())
            {
                assetData[key] =
                    it.value().get<bool>() ? "true" : "false";
            }
            else if (it.value().is_number())
            {
                assetData[key] = it.value().dump();
            }
            else
            {
                dbgln("Unsupported asset data type for key: " + key,
                    LogType::WARNING);
            }
        }
    }
    else
        dbgln((assetJson.contains("data") ? "Asset did not contain any data" : "Asset data was not an array") + std::string(", skipping..."), LogType::WARNING);

    return assetData;
}

// Gathers data from an asset JSON object, which should contain name, type, and path strings, returns a ProjectAssetData struct
ProjectAssetData GetAsset(const nlohmann::json_abi_v3_12_0::json &assetJson)
{
    // Gather the values
    const auto &assetName = assetJson.value("name", "");
    const auto &assetType = assetJson.value("type", "");
    const auto &assetPath = assetJson.value("path", "");

    dbgln("Loading asset: " + assetName, LogType::INFO);
    dbgln("Asset type: " + assetType, LogType::INFO);
    dbgln("Asset path: " + assetPath, LogType::INFO);

    // Assign them to a ProjectAssetData struct and return it
    ProjectAssetData assetData;
    assetData.name = assetName;
    assetData.type = assetType;
    assetData.path = assetPath;
    assetData.data = GetAssetData(assetJson);

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
                dbgln("Loading asset from file: " + assetFilePath, LogType::INFO);

                nlohmann::json assetJsonFromFile = ReadFileToJsonObject("assets/project/assets/" + assetFilePath); // Read the file into a string, and parse it into a JSON object
                assets.push_back(GetAsset(assetJsonFromFile)); // Gather the asset data and add it to the assets vector
            }
            else if (assetJson.is_object()) // If it's a normal JSON object, we can just gather the data directly
                assets.push_back(GetAsset(assetJson));
            else // If it's neither, log a warning and skip it
                dbgln("Asset entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
        }
    }
    else // If not, log a message and continue forward
        if (!projectJson.contains("assets"))
            dbgln("Project did not contain an assets property", LogType::WARNING);
        else if (!projectJson["assets"].is_array())
            dbgln("Project assets property was not an array", LogType::WARNING);
        else if (projectJson["assets"].empty())
            dbgln("Project had no assets in the assets property array", LogType::WARNING);
        else
            dbgln("Something else went wrong with loading assets", LogType::WARNING);

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
                dbgln("Scene entry was neither a string (file path) nor an object, skipping...", LogType::WARNING);
                continue;
            }

            if (sceneName == "main")
            {
                dbgln("Main scene found: " + sceneName + " at index " + std::to_string(i), LogType::INFO);
                return i;
            }
        }
    }
    else
        dbgln("[WARNING] Project did not contain any scenes to search for the main scene", LogType::WARNING);
    return 0; // Return -1 if no main scene is found
}

// Unpacks a project file from a JSON file path into a project object, returns the Project struct
Project lapCore::UnpackProject(const std::string projectJsonString)
{
    nlohmann::json projectJson = nlohmann::json::parse(projectJsonString); // Parse the JSON string into a JSON object
    std::string projectName = projectJson.value("name", "Unnamed Project");
    dbgln("Unpacking project: " + projectName, LogType::INFO);

    std::string projectVersion = projectJson.value("version", "0.0");
    dbgln("Version: " + projectVersion, LogType::INFO);

    Project project; // Create the project object, asign its values
    project.name = projectName;
    project.version = projectVersion;

    project.main_scene_index = GetMainScene(projectJson);

    project.assets = GetAssets(projectJson); // Gather the data from the JSON
    project.prefabs = GetPrefabs(projectJson);
    project.scenes = GetScenes(projectJson, project.prefabs);
    
    return project;
}

void lapCore::Project::RegisterDefaultElements()
{
    // ELEMENTS
    Reflection::Register<Transform2D>("transform-2d");
    Reflection::Register<Physics2D>("physics-2d");
    Reflection::Register<UIFrame>("ui-frame");
    Reflection::Register<UIList>("ui-list");
    Reflection::Register<Sprite>("sprite");
    Reflection::Register<UIImage>("ui-image");
    Reflection::Register<UITextLabel>("ui-text-label");
    Reflection::Register<UIButton>("ui-button");
    Reflection::Register<Script>("script");

    // EUTIL
    Reflection::Register<HorizontalAlignment>("horizontal-alignment");
    Reflection::Register<VerticalAlignment>("vertical-alignment");
    Reflection::Register<Alignment>("alignment");
    Reflection::Register<Renderable>("renderable");
    Reflection::Register<Axis2D>("axis-2d");
    Reflection::Register<ScrollSettings>("scroll-settings");
    Reflection::Register<Animated>("animated");
    Reflection::Register<Direction2D>("direction-2d");
    Reflection::Register<FrameVector>("frame-vector");
    Reflection::Register<Padding>("padding");
    Reflection::Register<UIOrigin>("ui-origin");

    // RAYLIB
    Reflection::Register<Camera2D>("camera-2d");
    Reflection::Register<Vector2>("vector2");
    Reflection::Register<Color>("color");

    // BOX2D
    Reflection::Register<b2BodyDef>("b2-body-def");
    Reflection::Register<b2ShapeDef>("b2-shape-def");
    Reflection::Register<b2Polygon>("b2-polygon");
    Reflection::Register<b2Filter>("b2-filter");
    Reflection::Register<b2SurfaceMaterial>("b2-surface-material");
    Reflection::Register<b2MotionLocks>("b2-motion-locks");
    Reflection::Register<b2BodyType>("b2-body-type");
    Reflection::Register<b2Vec2>("b2-vec2");
    Reflection::Register<b2Rot>("b2-rot");
}
