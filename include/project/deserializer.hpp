#pragma once

#include <nlohmann/json.hpp>
#include "project/project.hpp"

namespace lapCore
{
    using json = nlohmann::json;
    using ElementDataPtr = std::unique_ptr<IProjectElementData>;

    std::vector<ProjectAssetData> MakeAssets(const json &projectJson);
    ProjectAssetData MakeAsset(const json &assetJson);

    std::vector<ProjectObjectData> MakeObjects(const json &projectJson, bool prefab = false);
    ProjectObjectData MakeObject(const json &objectJson, bool prefab = false);

    std::vector<ElementDataPtr> MakeElements(const json &projectJson);
    ElementDataPtr MakeElement(const json &elementJson);

    std::vector<ProjectSceneData> MakeScenes(const json &projectJson);
    ProjectSceneData MakeScene(const json &sceneJson);

    void MakeInstancedPrefabs(const json &projectJson);
    void MakeInstancedPrefab(const json &prefabJson);

    std::vector<ProjectSystemData> MakeSystems(const json &projectJson);
    ProjectSystemData MakeSystem(const json &systemJson);

    Project MakeProject(const json &projectJson);
}