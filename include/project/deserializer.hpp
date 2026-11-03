#pragma once

#include <nlohmann/json.hpp>
#include "project/project.hpp"

namespace lapCore
{
    using json = nlohmann::json;
    using ElementDataPtr = std::unique_ptr<IProjectElementData>;

    std::vector<ProjectAssetData> MakeAssets(const json &assetsJson);
    ProjectAssetData MakeAsset(const json &assetJson);

    std::vector<ProjectObjectData> MakeObjects(const json &objectsJson, bool prefab = false);
    ProjectObjectData MakeObject(const json &objectJson, bool prefab = false);

    std::vector<ElementDataPtr> MakeElements(const json &elementsJson);
    ElementDataPtr MakeElement(const json &elementJson);

    std::vector<ProjectSceneData> MakeScenes(const json &scenesJson, const vector<ProjectObjectData>& prefabs);
    ProjectSceneData MakeScene(const json &sceneJson, const vector<ProjectObjectData>& prefabs);

    std::vector<ProjectInstancedPrefabData> MakeInstancedPrefabs(
        const json& instancedPrefabsJson,
        const std::vector<ProjectObjectData>& prefabs
    );

    ProjectInstancedPrefabData MakeInstancedPrefab(
        const json& instancedPrefabJson,
        const std::vector<ProjectObjectData>& prefabs
    );

    std::vector<ProjectSystemData> MakeSystems(const json &systemsJson);
    ProjectSystemData MakeSystem(const json &systemJson);

    Project MakeProject(const json &projectJson);
}