#include "project/unpacker.hpp"
#include "json/unpacker.hpp"
#include "eutil.hpp"
#include <stdexcept>

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
            groupPath = fileJson.at("group").get<string>();
        }

        if (!fileJson.is_object())
        {
            throw runtime_error(
                "UnpackGroupedFile: Expected a JSON object."
            );
        }

        if (groupPath.generic_string() != "def")
            fileJson["group"] = groupPath.generic_string();

        return categoryPath / groupPath;
    }
    
    json UnpackArrayOrInline(const json &projectJson, const string &key, function<json(json&, filesystem::path, string)> unpackerFunction, filesystem::path startingPath, string optional)
    {
        if (!jsonContains(projectJson, key))
            return json();

        json resultJson = projectJson.at(key);

        // A file@ reference for an entire collection is relative to the
        // file that references it (e.g. project.json -> prefabs.json).
        // This is different from a reference to ONE prefab definition.
        if (wantsReplacement(resultJson))
        {
            const filesystem::path replacementPath = GetReplacementValue(resultJson);
            const filesystem::path path = startingPath / replacementPath;

            // Do NOT pass an unresolved collection reference to UnpackPrefab:
            // it would incorrectly look under prefabs/def/.
            if (!filesystem::is_regular_file(path))
            {
                throw runtime_error(
                    "UnpackArrayOrInline: Missing collection '" + key +
                    "' at: " + filesystem::absolute(path).string());
            }

            resultJson = UnpackFile(path);
            if (!resultJson.is_array() && !resultJson.is_object())
            {
                throw runtime_error(
                    "UnpackArrayOrInline: Collection '" + key +
                    "' must contain a JSON array or object: " +
                    filesystem::absolute(path).string());
            }

            startingPath = path.parent_path();
        }

        if (resultJson.is_array())
        {
            for (auto &itemJson : resultJson)
                itemJson = unpackerFunction(itemJson, startingPath, optional);
        }
        else if (resultJson.is_object() || resultJson.is_string())
        {
            resultJson = unpackerFunction(resultJson, startingPath, optional);
        }

        return resultJson;
    }

    filesystem::path GetDirectoryPath(json &fileJson, const filesystem::path &startingPath, const string &category, const string &key)
    {
        auto groupDirectory = UnpackGroupedFile(fileJson, startingPath, category);
        return groupDirectory / fileJson.at(key).get<string>();
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
    
    // Resolve children overrides while preserving their *keys*.
    // Accepted form: {"childName": {...}, "otherChild": "exclude"}
    // External "file@..." collections may also contain that keyed object or
    // an array of child entries with a name/prefab field.
    json UnpackChildOverrides(const json& parentJson,
                              filesystem::path startingPath)
    {
        if (!parentJson.is_object() || !parentJson.contains("children"))
            return json();

        json children = parentJson.at("children");
        if (wantsReplacement(children))
        {
            const filesystem::path path = startingPath / GetReplacementValue(children);
            if (filesystem::is_regular_file(path))
            {
                children = UnpackFile(path);
                startingPath = path.parent_path();
            }
        }

        json result = json::object();

        if (children.is_object())
        {
            for (const auto& [childName, overrideJson] : children.items())
            {
                if (overrideJson == "exclude" || overrideJson == "default" ||
                    overrideJson.is_null())
                {
                    result[childName] = overrideJson.is_null()
                        ? json("default") : overrideJson;
                    continue;
                }

                json child = overrideJson;
                result[childName] = UnpackChildPrefab(child, startingPath, childName);
            }
        }
        else if (children.is_array())
        {
            for (const auto& childEntry : children)
            {
                std::string childName;
                if (childEntry.is_object())
                    childName = childEntry.value("name", childEntry.value("prefab", ""));
                else if (wantsReplacement(childEntry))
                    childName = filesystem::path(GetReplacementValue(childEntry)).stem().string();

                else if (childEntry.is_string())
                {
                    // Bare child names in arrays mean "use the default".
                    childName = childEntry.get<string>();
                    result[childName] = "default";
                    continue;
                }

                if (childName.empty())
                    continue;

                json child = childEntry;
                result[childName] = UnpackChildPrefab(child, startingPath, childName);
            }
        }

        return result;
    }

    json UnpackInstancedPrefab(json &instancedPrefabJson,
                              const filesystem::path &startingPath,
                              const string &optional)
    {
        const auto directoryPath = GetDirectoryPath(
            instancedPrefabJson, startingPath, "prefabs", "prefab");

        // "exclude" and "default" are markers, not JSON objects to unpack.
        if (instancedPrefabJson.contains("modifications") &&
            instancedPrefabJson.at("modifications") != "exclude" &&
            instancedPrefabJson.at("modifications") != "default")
        {
            json modificationsJson = UnpackArrayOrInline(
                instancedPrefabJson, "modifications", UnpackPrefabModification,
                directoryPath, instancedPrefabJson.at("prefab").get<string>());
            if (!modificationsJson.is_null())
                instancedPrefabJson["modifications"] = std::move(modificationsJson);
        }

        // Preserve support for explicitly nested instance descriptors.
        if (instancedPrefabJson.contains("children"))
        {
            if (instancedPrefabJson.at("children").is_array())
            {
                json childrenJson = UnpackArrayOrInline(
                    instancedPrefabJson, "children", UnpackInstancedPrefab,
                    directoryPath, optional);
                if (!childrenJson.is_null())
                    instancedPrefabJson["children"] = std::move(childrenJson);
            }
            else
            {
                // Keyed children are modifications, not nested instance
                // descriptors with a required "prefab" property.
                json childrenJson = UnpackChildOverrides(
                    instancedPrefabJson, directoryPath);
                if (!childrenJson.is_null())
                    instancedPrefabJson["children"] = std::move(childrenJson);
            }
        }

        return instancedPrefabJson;
    }

    json UnpackPrefabModification(json &prefabModificationJson,
                                  const filesystem::path &startingPath,
                                  const string &optional)
    {
        auto groupDirectory = UnpackGroupedFile(
            prefabModificationJson, startingPath, "modifications");
        const auto directoryPath = groupDirectory / optional;

        // Child overrides are keyed by the prefab child's name. Treating the
        // map as a single child loses those keys and fails on "exclude".
        json childrenJson = UnpackChildOverrides(prefabModificationJson, directoryPath);
        if (!childrenJson.is_null())
            prefabModificationJson["children"] = std::move(childrenJson);

        json elementsJson = UnpackArrayOrInline(
            prefabModificationJson, "elements", UnpackElement,
            directoryPath, optional);
        if (!elementsJson.is_null())
            prefabModificationJson["elements"] = std::move(elementsJson);

        return prefabModificationJson;
    }

    json UnpackChildPrefab(json &childPrefabJson,
                           const filesystem::path &startingPath,
                           const string &optional)
    {
        // A child override can be a marker, an inline override object, or
        // a file reference to an override object.
        if (childPrefabJson == "exclude" || childPrefabJson == "default")
            return childPrefabJson;

        const auto groupDirectory = UnpackGroupedFile(
            childPrefabJson, startingPath, "children");
        const auto directoryPath = groupDirectory / optional;

        // Some formats wrap the child override in "modifications".
        if (childPrefabJson.contains("modifications") &&
            childPrefabJson.at("modifications") != "exclude" &&
            childPrefabJson.at("modifications") != "default")
        {
            json nestedMods = UnpackArrayOrInline(
                childPrefabJson, "modifications", UnpackPrefabModification,
                directoryPath, optional);
            if (!nestedMods.is_null())
                childPrefabJson["modifications"] = std::move(nestedMods);
        }

        json elementsJson = UnpackArrayOrInline(
            childPrefabJson, "elements", UnpackElement,
            directoryPath, optional);
        if (!elementsJson.is_null())
            childPrefabJson["elements"] = std::move(elementsJson);

        json childrenJson = UnpackChildOverrides(childPrefabJson, directoryPath);
        if (!childrenJson.is_null())
            childPrefabJson["children"] = std::move(childrenJson);

        return childPrefabJson;
    }

    json UnpackMultidirectoryProject(const filesystem::path &projectFilePath)
    {
        filesystem::path resolvedPath = projectFilePath;

        if (resolvedPath.is_relative())
        {
            resolvedPath =
                filesystem::path(GetApplicationDirectory()) / resolvedPath;
        }

        resolvedPath = resolvedPath.lexically_normal();

        json projectJson = UnpackFile(resolvedPath);

        // Collection files must be relative to project.json.
        filesystem::path rootPath = resolvedPath.parent_path();

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