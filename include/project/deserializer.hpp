#pragma once

#include <nlohmann/json.hpp>
#include "project/project.hpp"

namespace lapCore
{
    using namespace std;

    using json = nlohmann::json;
    using ElementDataPtr = unique_ptr<IProjectElementData>;

    vector<ProjectAssetData> MakeAssets(const json &assetsJson);
    ProjectAssetData MakeAsset(const json &assetJson);

    vector<ProjectObjectData> MakeObjects(const json &objectsJson, bool prefab = false);
    ProjectObjectData MakeObject(const json &objectJson, bool prefab = false);

    vector<ElementDataPtr> MakeElements(const json &elementsJson);
    ElementDataPtr MakeElement(const json &elementJson);

    vector<ProjectSceneData> MakeScenes(const json &scenesJson, const vector<ProjectObjectData>& prefabs);
    ProjectSceneData MakeScene(const json &sceneJson, const vector<ProjectObjectData>& prefabs);

    vector<ProjectInstancedPrefabData> MakeInstancedPrefabs(
        const json& instancedPrefabsJson,
        const vector<ProjectObjectData>& prefabs
    );

    ProjectInstancedPrefabData MakeInstancedPrefab(
        const json& instancedPrefabJson,
        const vector<ProjectObjectData>& prefabs
    );

    vector<ProjectSystemData> MakeSystems(const json &systemsJson);
    ProjectSystemData MakeSystem(const json &systemJson);

    Project MakeProject(const json &projectJson);
}