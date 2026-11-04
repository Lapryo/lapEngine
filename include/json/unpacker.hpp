#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>

using namespace std;

namespace lapCore
{
    using json = nlohmann::json;

    // Helper tools
    bool jsonContains(const json& j, const string& key, bool allowNull = false);
    bool jsonContainsArray(const json& j, const string& key, bool allowEmpty = false);
    bool jsonContainsObject(const json& j, const string& key, bool allowEmpty = false);
    bool jsonContainsString(const json& j, const string& key, bool allowEmpty = false);

    json UnpackFile(const filesystem::path& jsonFilePath);

    // Replacement functions
    bool wantsReplacement(const json& j);
    string GetReplacementValue(const json &j, const string &key);
    string GetReplacementValue(const json &j);

    json ReplaceWithFile(json& j, const filesystem::path& filePath);
    json ReplaceKeyValueWithFile(json& j, const string& key, const filesystem::path& filePath);
}