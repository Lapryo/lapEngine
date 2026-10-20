#include "project/unpacker.hpp"
#include "json/unpacker.hpp"
#include "eutil.hpp"

namespace lapCore
{
    using namespace EUTIL;
    using namespace Functions;
    using namespace Other;

    filesystem::path GetGroupPath(const filesystem::path &replacementPath)
    {
        auto groupPath = replacementPath.parent_path();
        if (groupPath.empty())
            return "def";

        return groupPath;
    }
    filesystem::path UnpackGroupedFile(json &fileJson, const filesystem::path &startingPath, const string &category)
    {
        auto categoryPath = startingPath / category;
        filesystem::path groupPath = "def";

        if (wantsReplacement(fileJson))
        {
            filesystem::path replacementPath = GetReplacementValue(fileJson);

            groupPath = GetGroupPath(replacementPath);

            auto path = categoryPath / groupPath / replacementPath.filename();

            fileJson = UnpackFile(path);
        }
        else if (fileJson.is_object() && fileJson.contains("group"))
        {
            groupPath = fileJson.at("group").get<std::string>();
        }

        if (!fileJson.is_object())
        {
            throw std::runtime_error(
                "UnpackGroupedFile: Expected a JSON object."
            );
        }

        if (groupPath.generic_string() != "def")
            fileJson["group"] = groupPath.generic_string();

        return categoryPath / groupPath;
    }
    
    json UnpackArrayOrInline(const json &projectJson, const std::string &key, function<json(json&, filesystem::path, string)> unpackerFunction, filesystem::path startingPath, string optional)
    {
        if (!jsonContains(projectJson, key))
            return json();

        json resultJson = projectJson[key];

        // Check if the entire value references an external JSON array.
        if (wantsReplacement(resultJson))
        {
            filesystem::path replacementPath = GetReplacementValue(resultJson);

            // Collection files are relative to the starting directory.
            auto path = startingPath / replacementPath;

            if (filesystem::is_regular_file(path))
            {
                json loadedJson = UnpackFile(path);

                if (loadedJson.is_array())
                {
                    resultJson = loadedJson;

                    // Use the collection file's directory as the
                    // starting directory for its contained entries.
                    startingPath = path.parent_path();
                }
            }
        }

        if (resultJson.is_array())
            for (auto &itemJson : resultJson)
                itemJson = unpackerFunction(itemJson, startingPath, optional);
        else if (resultJson.is_object() || resultJson.is_string())
            resultJson = unpackerFunction(resultJson, startingPath, optional);

        return resultJson;
    }

    filesystem::path GetDirectoryPath(json &fileJson, const filesystem::path &startingPath, const string &category, const std::string &key)
    {
        auto groupDirectory = UnpackGroupedFile(fileJson, startingPath, category);
        return groupDirectory / fileJson.at(key).get<std::string>();
    }

    // Unpacking
    
    json UnpackAsset(json &assetJson, const filesystem::path &startingPath, const string &optional)
    {
        UnpackGroupedFile(assetJson, startingPath, "assets");
        return assetJson;
    }
    json UnpackSystem(json &systemJson, const filesystem::path &startingPath, const string &optional)
    {
        UnpackGroupedFile(systemJson, startingPath, "systems");
        return systemJson;
    }
    json UnpackElement(json &elementJson, const filesystem::path &startingPath, const string &optional)
    {
        UnpackGroupedFile(elementJson, startingPath, "elements");
        return elementJson;
    }

    json UnpackPrefab(json &prefabJson, const filesystem::path &startingPath, const string &optional)
    {
        json elementsJson = UnpackArrayOrInline(prefabJson, "elements", UnpackElement, GetDirectoryPath(prefabJson, startingPath, "prefabs", "name"), optional);

        if (!elementsJson.is_null())
            prefabJson["elements"] = elementsJson;

        return prefabJson;
    }
    
    json UnpackScene(json &sceneJson, const filesystem::path &startingPath, const string &optional)
    {
        auto directoryPath = GetDirectoryPath(sceneJson, startingPath, "scenes", "name");

        json systemsJson = UnpackArrayOrInline(sceneJson, "systems", UnpackSystem, directoryPath, optional);
        if (!systemsJson.is_null())
            sceneJson["systems"] = systemsJson;

        json objectsJson = UnpackArrayOrInline(sceneJson, "objects", UnpackObject, directoryPath, optional);
        if (!objectsJson.is_null())
            sceneJson["objects"] = objectsJson;

        json instancedPrefabsJson = UnpackArrayOrInline(sceneJson, "prefabs", UnpackInstancedPrefab, directoryPath, optional);
        if (!instancedPrefabsJson.is_null())
            sceneJson["prefabs"] = instancedPrefabsJson;

        return sceneJson;
    }
    
    json UnpackObject(json &objectJson, const filesystem::path &startingPath, const string &optional)
    {
        json elementsJson = UnpackArrayOrInline(objectJson, "elements", UnpackElement, GetDirectoryPath(objectJson, startingPath, "objects", "name"), optional);
        if (!elementsJson.is_null())
            objectJson["elements"] = elementsJson;

        return objectJson;
    }
    
    json UnpackInstancedPrefab(json &instancedPrefabJson, const filesystem::path &startingPath, const string &optional)
    {
        // Very complicated!
        json modificationsJson = UnpackArrayOrInline(
            instancedPrefabJson, "modifications", UnpackPrefabModification, 
            GetDirectoryPath(instancedPrefabJson, startingPath, "prefabs", "name"), 
            instancedPrefabJson.at("prefab").get<std::string>()
        );
        if (!modificationsJson.is_null())
            instancedPrefabJson["modifications"] = modificationsJson;

        return instancedPrefabJson;
    }

    json UnpackPrefabModification(json &prefabModificationJson, const filesystem::path &startingPath, const string &optional)
    {
        // Because the prefab name isn't a key of the modification JSON, we have to get it from the optional string
        // There's probably a better way to do this, but whatever
        auto groupDirectory = UnpackGroupedFile(prefabModificationJson, startingPath, "modifications");
        auto directoryPath = groupDirectory / optional;

        json childrenJson = UnpackArrayOrInline(prefabModificationJson, "children", UnpackChildPrefab, directoryPath, optional);
        if (!childrenJson.is_null())
            prefabModificationJson["children"] = childrenJson;

        json elementsJson = UnpackArrayOrInline(prefabModificationJson, "elements", UnpackElement, directoryPath, optional);
        if (!elementsJson.is_null())
            prefabModificationJson["elements"] = elementsJson;

        return prefabModificationJson;
    }

    json UnpackChildPrefab(json &childPrefabJson, const filesystem::path &startingPath, const string &optional)
    {
        // Like before, the prefab name isn't a key of the child prefab JSON, prefabs are very complicated!
        auto groupDirectory = UnpackGroupedFile(childPrefabJson, startingPath, "children");
        auto directoryPath = groupDirectory / optional;

        json elementsJson = UnpackArrayOrInline(childPrefabJson, "elements", UnpackElement, directoryPath, optional);
        if (!elementsJson.is_null())
            childPrefabJson["elements"] = elementsJson;

        json childrenJson = UnpackArrayOrInline(childPrefabJson, "children", UnpackChildPrefab, directoryPath, optional);
        if (!childrenJson.is_null())
            childPrefabJson["children"] = childrenJson;

        return childPrefabJson;
    }

    json UnpackMultidirectoryProject(const filesystem::path &projectFilePath)
    {
        json projectJson = UnpackFile(projectFilePath);
        if (projectJson.is_null())
        {
            dbgln("UnpackMultidirectoryProject: Failed to unpack project file: " + projectFilePath.string() + "\nReturning null JSON.", LogType::ERROR);
            return json();
        }

        auto rootPath = projectFilePath.parent_path();

        json assetsJson = UnpackArrayOrInline(projectJson, "assets", UnpackAsset, rootPath, "");
        if (!assetsJson.is_null())
            projectJson["assets"] = assetsJson;

        json prefabsJson = UnpackArrayOrInline(projectJson, "prefabs", UnpackPrefab, rootPath, "");
        if (!prefabsJson.is_null())
            projectJson["prefabs"] = prefabsJson;

        json scenesJson = UnpackArrayOrInline(projectJson, "scenes", UnpackScene, rootPath, "");
        if (!scenesJson.is_null())
            projectJson["scenes"] = scenesJson;

        return projectJson;
    }
}