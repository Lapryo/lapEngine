#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>

using namespace std;

namespace lapCore
{
    using json = nlohmann::json;

    // Helpers
    filesystem::path GetGroupPath(const filesystem::path &replacementPath);
    filesystem::path UnpackGroupedFile(const json &fileJson, const filesystem::path &startingPath, const string &category);
    filesystem::path GetDirectoryPath(json &fileJson, const filesystem::path &startingPath, const string &category, const std::string &key);

    json UnpackArrayOrInline(json &projectJson, const std::string &key, function<json(json&, filesystem::path, string)> unpackerFunction, filesystem::path startingPath, string optional);

    // Unpacking
    
    json UnpackAsset(json &assetJson, const filesystem::path &startingPath, const string &optional);
    json UnpackPrefab(json &prefabJson, const filesystem::path &startingPath, const string &optional);
    
    json UnpackScene(json &sceneJson, const filesystem::path &startingPath, const string &optional);
    json UnpackSystem(json &systemJson, const filesystem::path &startingPath, const string &optional);
    
    json UnpackObject(json &objectJson, const filesystem::path &startingPath, const string &optional);
    json UnpackElement(json &elementJson, const filesystem::path &startingPath, const string &optional);
    
    json UnpackInstancedPrefab(json &instancedPrefabJson, const filesystem::path &startingPath, const string &optional);
    json UnpackPrefabModification(json &prefabModificationJson, const filesystem::path &startingPath, const string &optional);
    json UnpackChildPrefab(json &childPrefabJson, const filesystem::path &startingPath, const string &optional);

    json UnpackMultidirectoryProject(const filesystem::path &projectFilePath);
}