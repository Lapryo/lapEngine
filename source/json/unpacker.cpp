#include "json/unpacker.hpp"
#include "eutil.hpp"

namespace lapCore
{
    using namespace EUTIL::Functions;

    const char* REPLACEMENT_KEYWORD = "file@";

    bool jsonContains(const json &j, const string &key, bool allowNull)
    {
        if (allowNull)
            return j.contains(key);
        return j.contains(key) && !j[key].is_null();
    }

    bool jsonContainsArray(const json &j, const string &key, bool allowEmpty)
    {
        if (allowEmpty)
            return jsonContains(j, key, true) && j[key].is_array();
        return jsonContainsArray(j, key, false) && !j[key].empty();
    }

    bool jsonContainsObject(const json &j, const string &key, bool allowEmpty)
    {
        if (allowEmpty)
            return jsonContains(j, key, true) && j[key].is_object();
        return jsonContainsObject(j, key, true) && !j[key].empty();
    }

    bool jsonContainsString(const json &j, const string &key, bool allowEmpty)
    {
        if (allowEmpty)
            return jsonContains(j, key, true) && j[key].is_string();
        return jsonContainsString(j, key, true) && !j[key].empty();
    }

    json UnpackFile(const filesystem::path& path)
    {
        filesystem::path resolvedPath = path;

        if (resolvedPath.is_relative())
        {
            resolvedPath =
                filesystem::path(GetApplicationDirectory()) / resolvedPath;
        }

        resolvedPath = resolvedPath.lexically_normal();

        std::ifstream file(resolvedPath);

        if (!file.is_open())
        {
            dbgln(
                "UnpackFile: Failed to read JSON file: " +
                resolvedPath.string(),
                EUTIL::Other::LogType::ERROR
            );

            return nullptr;
        }

        return json::parse(file);
    }

    bool wantsReplacement(const json &j)
    {
        if (j.is_string())
        {
            if (j.get<string>().substr(0, strlen(REPLACEMENT_KEYWORD)) == REPLACEMENT_KEYWORD)
                return true;
        }

        return false;
    }

    string GetReplacementValue(const json &j, const string &key)
    {
        // Get the characters after the replacement keyword
        return j[key].get<string>().substr(strlen(REPLACEMENT_KEYWORD));
    }

    string GetReplacementValue(const json &j)
    {
        // Get the characters after the replacement keyword
        return j.get<string>().substr(strlen(REPLACEMENT_KEYWORD));
    }

    json ReplaceWithFile(json &j, const filesystem::path &filePath)
    {
        if (wantsReplacement(j))
        {
            auto relativePath = GetReplacementValue(j, "");
            auto path = filePath.parent_path() / relativePath;

            j = UnpackFile(path);
            return j;
        }

        return json();
    }

    json ReplaceKeyValueWithFile(json &j, const string &key, const filesystem::path &filePath)
    {
        if (jsonContains(j, key) && wantsReplacement(j[key]))
        {
            auto relativePath = GetReplacementValue(j, key);
            auto path = filePath.parent_path() / relativePath;

            auto replacementJson = UnpackFile(path);
            if (!replacementJson.is_null())
            {
                j[key] = replacementJson;
                return replacementJson;
            }
        }

        return json();
    }

    json ReplaceElementsWithFiles(json &jsonArray, const filesystem::path &filePath)
    {
        for (auto &element : jsonArray)
        {
            if (wantsReplacement(element))
            {
                ReplaceWithFile(element, filePath);
            }
        }

        return jsonArray;
    }

}