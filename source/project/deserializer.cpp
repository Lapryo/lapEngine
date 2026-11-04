#include "project/deserializer.hpp"

#include "reflection.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace lapCore;
using namespace std;

namespace lapCore
{
    Project MakeProject(const json &projectJson)
    {
        Project project;

        project.name = projectJson.value("name", "Default Project");

        if (projectJson.contains("assets"))
            project.assets = MakeAssets(projectJson.at("assets"));
        if (projectJson.contains("prefabs"))
            project.prefabs = MakeObjects(projectJson.at("prefabs"), true);
        if (projectJson.contains("scenes"))
            project.scenes = MakeScenes(projectJson.at("scenes"), project.prefabs);

        return project;
    }

    vector<ProjectAssetData> MakeAssets(const json &assetsJson)
    {
        vector<ProjectAssetData> assets;

        for (const auto &assetJson : assetsJson)
            if (assetJson.is_object())
                assets.push_back(MakeAsset(assetJson));

        return assets;
    }

    ProjectAssetData MakeAsset(const json &assetJson)
    {
        ProjectAssetData asset;

        asset.name = assetJson.value("name", "unknown");
        asset.type = assetJson.value("type", "unknown");
        asset.path = assetJson.value("path", "unknown");
        
        if (assetJson.contains("data"))
        {
            for (const auto &[key, value] : assetJson.at("data").items())
            {
                asset.data[key] = value;
            }
        }

        return asset;
    }

    vector<ProjectObjectData> MakeObjects(const json &objectsJson, bool prefab)
    {
        vector<ProjectObjectData> objects;

        for (const auto &objectJson : objectsJson)
            if (objectJson.is_object())
                objects.push_back(MakeObject(objectJson, prefab));

        return objects;
    }

    ProjectObjectData MakeObject(const json &objectJson, bool prefab)
    {
        ProjectObjectData object;

        object.name = objectJson.value("name", "unknown");
        object.parent = objectJson.value("parent", "");
        object.index = objectJson.value("child-index", objectJson.value("index", -1));

        if (objectJson.contains("elements"))
            object.elements = MakeElements(objectJson.at("elements"));

        return object;
    }

    vector<ElementDataPtr> MakeElements(const json &elementsJson)
    {
        vector<ElementDataPtr> elements;

        for (const auto &elementJson : elementsJson)
            if (elementJson.is_object())
            {
                auto element = MakeElement(elementJson);
                if (element)
                    elements.push_back(std::move(element));
            }

        return elements;
    }

    ElementDataPtr MakeElement(const json &elementJson)
    {
        auto type = elementJson.value("type", "");
        auto entry = Reflection::TryGet(type);
        if (!entry) return nullptr;

        if (!elementJson.contains("data") || !elementJson.at("data").is_object())
            return nullptr;

        auto element = entry->create_project_data(elementJson.at("data"), type);
        if (!element) return nullptr;
        element->sourceJson = elementJson;

        return element;
    }
    
    vector<ProjectSceneData> MakeScenes(const json &scenesJson, const vector<ProjectObjectData> &prefabs)
    {
        vector<ProjectSceneData> scenes;

        for (const auto &sceneJson : scenesJson)
            if (sceneJson.is_object())
                scenes.push_back(MakeScene(sceneJson, prefabs));

        return scenes;
    }

    ProjectSceneData MakeScene(const json &sceneJson, const vector<ProjectObjectData> &prefabs)
    {
        ProjectSceneData scene;

        scene.name = sceneJson.value("name", "unknown");

        if (sceneJson.contains("objects"))
            scene.objects = MakeObjects(sceneJson.at("objects"), false);
        if (sceneJson.contains("background-color"))
            JSON::Serializer<Color>::from_json(scene.backgroundColor, sceneJson.at("background-color"));
        if (sceneJson.contains("prefabs"))
            scene.instancedPrefabs = MakeInstancedPrefabs(sceneJson.at("prefabs"), prefabs);
        if (sceneJson.contains("systems"))
            scene.systems = MakeSystems(sceneJson.at("systems"));

        return scene;
    }

    ElementDataPtr CreateModifiedElement(const json &elementModificationJson,
                                        const ProjectObjectData &baseData)
    {
        if (!elementModificationJson.is_object())
            return nullptr;

        const std::string type = elementModificationJson.value("type", "");
        auto entry = Reflection::TryGet(type);
        if (!entry)
            return nullptr;

        const auto typeLookup = Reflection::reverseLookup.find(type);
        if (typeLookup == Reflection::reverseLookup.end())
            return nullptr;

        ElementDataPtr existing;
        for (const auto &element : baseData.elements)
        {
            if (element && element->GetTypeID() == typeLookup->second)
            {
                existing = element->Clone();
                break;
            }
        }

        if (!elementModificationJson.contains("data"))
            return existing;

        const auto &overrideData = elementModificationJson.at("data");
        if (overrideData.is_string() && overrideData.get<std::string>() == "exclude")
            return nullptr;

        if (!overrideData.is_object())
            return existing;

        json merged = json::object();
        if (existing && existing->sourceJson.is_object() &&
            existing->sourceJson.contains("data") &&
            existing->sourceJson.at("data").is_object())
        {
            merged = existing->sourceJson.at("data");
        }

        // Merge into a clone of the original data, rather than replacing
        // unspecified fields with reflection defaults.
        // nlohmann::json's merge_patch mutates the target in place.
        // This guarantees that {"text": "test"} overwrites the
        // inherited ui-text-label text, regardless of the behavior
        // of the project's JSON::Merge helper.
        merged.merge_patch(overrideData);
        auto result = entry->create_project_data(merged, type);
        if (!result)
            return nullptr;

        result->sourceJson = existing ? existing->sourceJson : json::object();
        result->sourceJson["type"] = type;
        result->sourceJson["data"] = std::move(merged);
        return result;
    }

    ProjectObjectData ApplyModifications(const ProjectObjectData &base,
                                         const json &modifications)
    {
        ProjectObjectData modified = base; // Deep copy; preserve inherited elements.
        if (!modifications.is_object())
            return modified;

        modified.name = modifications.value("name", modified.name);
        modified.parent = modifications.value("parent", modified.parent);
        modified.index = modifications.value(
            "child-index", modifications.value("index", modified.index));

        if (!modifications.contains("elements") ||
            !modifications.at("elements").is_array())
            return modified;

        for (const auto &overrideElement : modifications.at("elements"))
        {
            if (!overrideElement.is_object())
                continue;

            const std::string type = overrideElement.value("type", "");
            if (!Reflection::TryGet(type))
                continue;

            const auto typeLookup = Reflection::reverseLookup.find(type);
            if (typeLookup == Reflection::reverseLookup.end())
                continue;

            const auto existing = std::find_if(
                modified.elements.begin(), modified.elements.end(),
                [&](const ElementDataPtr &element)
                {
                    return element && element->GetTypeID() == typeLookup->second;
                });

            // Use the *current* object so successive overrides are cumulative.
            auto updated = CreateModifiedElement(overrideElement, modified);

            if (existing != modified.elements.end())
            {
                if (updated)
                    *existing = std::move(updated);
                else if (overrideElement.contains("data") &&
                         overrideElement.at("data") == "exclude")
                    modified.elements.erase(existing);
            }
            else if (updated)
            {
                modified.elements.push_back(std::move(updated));
            }
        }
        return modified;
    }

    namespace
    {
        const ProjectObjectData* FindPrefab(const std::vector<ProjectObjectData>& prefabs,
                                            const std::string& name)
        {
            for (const auto& prefab : prefabs)
                if (prefab.name == name)
                    return &prefab;
            return nullptr;
        }

        // The definitions are flat. A prefab whose parent is "Enemy" is a
        // default descendant of the prefab named "Enemy".
        void BuildPrefabChildren(ProjectInstancedPrefabData& instance,
                                 const std::vector<ProjectObjectData>& prefabs,
                                 const json& childOverrides,
                                 std::vector<std::string>& ancestry)
        {
            for (const auto& childPrefab : prefabs)
            {
                if (childPrefab.parent != instance.prefabName)
                    continue;

                if (std::find(ancestry.begin(), ancestry.end(), childPrefab.name)
                    != ancestry.end())
                {
                    std::cerr << "Prefab hierarchy cycle at: "
                              << childPrefab.name << '\n';
                    continue;
                }

                json childMods = json::object();
                if (childOverrides.is_object() &&
                    childOverrides.contains(childPrefab.name))
                {
                    childMods = childOverrides.at(childPrefab.name);
                }

                if (childMods == "exclude")
                    continue;
                if (childMods == "default")
                    childMods = json::object();
                if (!childMods.is_object())
                    continue;

                // Also accept { "modifications": {...} } wrappers.
                if (childMods.contains("modifications"))
                    childMods = childMods.at("modifications");
                if (childMods == "exclude")
                    continue;
                if (!childMods.is_object())
                    childMods = json::object();

                ProjectInstancedPrefabData child{};
                child.prefabName = childPrefab.name;
                child.objectData = ApplyModifications(childPrefab, childMods);

                // Default parent is the *instance*, not the prefab template.
                // Explicit parent overrides, including "", remain untouched.
                if (!childMods.contains("parent"))
                    child.objectData.parent = instance.objectData.name;

                ancestry.push_back(childPrefab.name);
                const json grandchildren = childMods.value("children", json::object());
                BuildPrefabChildren(child, prefabs, grandchildren, ancestry);
                ancestry.pop_back();

                instance.children.push_back(std::move(child));
            }

            // Previously, misspelled/missing child names were silently
            // ignored. Warn when an override does not match a direct child
            // of the current prefab template.
            if (childOverrides.is_object())
            {
                for (const auto& [childName, childOverride] : childOverrides.items())
                {
                    const bool matched = std::any_of(
                        prefabs.begin(), prefabs.end(),
                        [&](const ProjectObjectData& candidate)
                        {
                            return candidate.parent == instance.prefabName &&
                                   candidate.name == childName;
                        });

                    if (!matched)
                    {
                        std::cerr << "No child prefab named '" << childName
                                  << "' under '" << instance.prefabName
                                  << "'. Its override was ignored.\n";
                    }
                }
            }
        }
    }

    ProjectInstancedPrefabData MakeInstancedPrefab(
        const json &instancedPrefabJson,
        const vector<ProjectObjectData> &prefabs)
    {
        ProjectInstancedPrefabData result{};
        if (!instancedPrefabJson.is_object())
            return result;

        result.prefabName = instancedPrefabJson.value("prefab", "");
        const auto* prefab = FindPrefab(prefabs, result.prefabName);
        if (!prefab)
        {
            std::cerr << "Unknown prefab: " << result.prefabName << '\n';
            return {};
        }

        const json modifications = instancedPrefabJson.value(
            "modifications", json::object());

        if (modifications == "exclude")
            return {};

        const json normalizedMods = modifications.is_object()
            ? modifications : json::object();

        result.objectData = ApplyModifications(*prefab, normalizedMods);

        // An instance can override its root name/parent/index. Children are
        // automatically derived from prefab definitions and then overridden
        // by the keyed `modifications.children` object.
        std::vector<std::string> ancestry{result.prefabName};
        json childrenMods = normalizedMods.value("children", json::object());
        // Also accept a top-level keyed "children" object for compatibility.
        if (instancedPrefabJson.contains("children") &&
            instancedPrefabJson.at("children").is_object())
        {
            if (!childrenMods.is_object())
                childrenMods = json::object();
            for (const auto& [name, value] : instancedPrefabJson.at("children").items())
                childrenMods[name] = value;
        }
        BuildPrefabChildren(result, prefabs, childrenMods, ancestry);

        // Backwards compatibility: explicit nested instance descriptors.
        if (instancedPrefabJson.contains("children") &&
            instancedPrefabJson.at("children").is_array())
        {
            for (const auto& childJson : instancedPrefabJson.at("children"))
            {
                if (!childJson.is_object())
                    continue;

                auto child = MakeInstancedPrefab(childJson, prefabs);
                if (child.prefabName.empty())
                    continue;

                const auto& explicitMods = childJson.value(
                    "modifications", json::object());
                if (!explicitMods.is_object() || !explicitMods.contains("parent"))
                    child.objectData.parent = result.objectData.name;

                result.children.push_back(std::move(child));
            }
        }

        return result;
    }

    vector<ProjectInstancedPrefabData> MakeInstancedPrefabs(
        const json &instancedPrefabsJson,
        const vector<ProjectObjectData> &prefabs)
    {
        vector<ProjectInstancedPrefabData> instances;
        if (!instancedPrefabsJson.is_array())
            return instances;

        for (const auto& entry : instancedPrefabsJson)
        {
            if (!entry.is_object())
                continue;

            const int count = entry.value("count", 1);
            if (count <= 0)
                continue;

            for (int i = 0; i < count; ++i)
            {
                json instanceJson = entry;
                if (count > 1)
                {
                    json modifications = instanceJson.value(
                        "modifications", json::object());
                    if (modifications == "exclude")
                        break;
                    if (!modifications.is_object())
                        modifications = json::object();

                    const auto baseName = modifications.value(
                        "name", entry.value("prefab", std::string("prefab")));
                    modifications["name"] = baseName + std::to_string(i + 1);
                    instanceJson["modifications"] = std::move(modifications);
                }

                auto instance = MakeInstancedPrefab(instanceJson, prefabs);
                if (!instance.prefabName.empty())
                    instances.push_back(std::move(instance));
            }
        }
        return instances;
    }

    vector<ProjectSystemData> MakeSystems(const json &systemsJson)
    {
        vector<ProjectSystemData> systems;

        for (const auto &systemJson : systemsJson)
            if (systemJson.is_object())
                systems.push_back(MakeSystem(systemJson));

        return systems;
    }

    ProjectSystemData MakeSystem(const json &systemJson)
    {
        ProjectSystemData system;

        system.type = systemJson.value("type", "unknown");
        system.order = systemJson.value("order", -1);

        return system;
    }
}
