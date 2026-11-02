#include "project/deserializer.hpp"

#include "reflection.hpp"

using namespace lapCore;

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
            project.scenes = MakeScenes(projectJson.at("scenes"));

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
        object.index = objectJson.value("index", -1);

        if (objectJson.contains("elements"))
            object.elements = MakeElements(objectJson.at("elements"));

        return object;
    }

    vector<ElementDataPtr> MakeElements(const json &elementsJson)
    {
        vector<ElementDataPtr> elements;

        for (const auto &elementJson : elementsJson)
            if (elementJson.is_object())
                elements.push_back(MakeElement(elementJson));

        return elements;
    }

    ElementDataPtr MakeElement(const json &elementJson)
    {
        auto type = elementJson.value("type", "");
        auto entry = Reflection::TryGet(type);
        if (!entry) return nullptr;

        auto element = entry->create_project_data(elementJson.at("data"), type);
        element->sourceJson = elementJson;

        return element;
    }
    
    vector<ProjectSceneData> MakeScenes(const json &scenesJson)
    {
        vector<ProjectSceneData> scenes;

        for (const auto &sceneJson : scenesJson)
            if (sceneJson.is_object())
                scenes.push_back(MakeScene(sceneJson));

        return scenes;
    }

    ProjectSceneData MakeScene(const json &sceneJson)
    {
        ProjectSceneData scene;

        scene.name = sceneJson.value("name", "unknown");

        if (sceneJson.contains("objects"))
            scene.objects = MakeObjects(sceneJson.at("objects"), false);
        if (sceneJson.contains("background-color"))
            JSON::Serializer<Color>::from_json(scene.backgroundColor, sceneJson.at("background-color"));
        if (sceneJson.contains("prefabs"))
            scene.instancedPrefabs = MakeInstancedPrefabs(sceneJson.at("prefabs"));
        if (sceneJson.contains("systems"))
            scene.systems = MakeSystems(sceneJson.at("systems"));

        return scene;
    }

    ElementDataPtr CreateModifiedElement(const json &elementModificationJson, const ProjectObjectData &baseData)
    {
        auto elementType = elementModificationJson.value("type", "");
        auto entry = Reflection::TryGet(elementType);
        if (!entry) return nullptr;

        ElementDataPtr elementData = nullptr;
        for (auto& element : baseData.elements)
        {
            if (element->GetTypeID() == Reflection::reverseLookup.at(elementType))
            {
                // We must copy the data to the elementData
                elementData = element->Clone();
                break;
            }
        }

        if (elementModificationJson.contains("data"))
        {
            if (!elementData && elementModificationJson.at("data").is_string() && elementModificationJson.at("data").get<std::string>() == "exclude")
            {
                // Then we do nothing, as the element already doesn't exist
            }
            else if (elementData && elementModificationJson.at("data").is_string() && elementModificationJson.at("data").get<std::string>() == "exclude")
            {
                // We must remove the element from the modified object
                elementData = nullptr;
            }
            else if (!elementData && elementModificationJson.at("data").is_object())
            {
                // Create a new element with the provided data
                elementData = entry->create_project_data(elementModificationJson.at("data"), elementType);
            }
            else if (elementData && elementModificationJson.at("data").is_object())
            {
                // Update the existing element with the new data
                auto merged = elementData->sourceJson;
                JSON::Merge(merged.at("data"), elementModificationJson.at("data"));

                auto newElement = entry->create_project_data(merged.at("data"), elementType);
                elementData = std::move(newElement);
            }
        }

        return elementData;
    }

    ProjectObjectData ApplyModifications(const ProjectObjectData &base, const json &modifications)
    {
        ProjectObjectData modified = base;
        modified.elements.clear();

        modified.name = modifications.value("name", modified.name);
        modified.parent = modifications.value("parent", modified.parent);
        modified.index = modifications.value("index", modified.index);
        
        // Apply modifications to elements here
        if (modifications.contains("elements"))
            for (const auto &elementModification : modifications.at("elements"))
                modified.elements.push_back(CreateModifiedElement(elementModification, base));

        return modified;
    }

    ProjectInstancedPrefabData MakeInstancedPrefab(const json &instancedPrefabJson, const vector<ProjectObjectData> &prefabs)
    {
        ProjectInstancedPrefabData instancedPrefab;

        // Get the name and initialize the base object data
        instancedPrefab.prefabName = instancedPrefabJson.value("prefab", "unknown");
        for (const auto &prefab : prefabs)
        {
            if (prefab.name == instancedPrefab.prefabName)
            {
                instancedPrefab.objectData = prefab;
                break;
            }
        }

        // Apply modifications if they exist
        if (instancedPrefabJson.contains("modifications"))
        {
            auto modificationsJson = instancedPrefabJson.at("modifications");
            if (modificationsJson.is_object())
            {
                instancedPrefab.objectData = ApplyModifications(instancedPrefab.objectData, modificationsJson);
            }
            else if (modificationsJson.is_string() && modificationsJson.get<std::string>() == "exclude")
            {
                return ProjectInstancedPrefabData();
            }
        }

        // Get the children of the prefab and do this current function recursively
        if (instancedPrefabJson.contains("children"))
        {
            for (const auto &childJson : instancedPrefabJson.at("children"))
            {
                ProjectInstancedPrefabData childData = MakeInstancedPrefab(childJson, prefabs);
                instancedPrefab.children.push_back(childData);
            }
        }

        return instancedPrefab;
    }

    vector<ProjectInstancedPrefabData> MakeInstancedPrefabs(const json &instancedPrefabsJson, const vector<ProjectObjectData> &prefabs)
    {
        vector<ProjectInstancedPrefabData> instancedPrefabs;

        for (const auto &instancedPrefabJson : instancedPrefabsJson)
            if (instancedPrefabJson.is_object())
                instancedPrefabs.push_back(MakeInstancedPrefab(instancedPrefabJson, prefabs));

        return instancedPrefabs;
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
