#include "json/unpacker.hpp"
#include "eutil.hpp"

namespace lapCore
{
    using namespace EUTIL::Functions;

    const char* REPLACEMENT_KEYWORD = "file@";

    bool jsonContains(const json &j, const std::string &key, bool allowNull)
    {
        if (allowNull)
            return j.contains(key);
        return j.contains(key) && !j[key].is_null();
    }

    bool jsonContainsArray(const json &j, const std::string &key, bool allowEmpty)
    {
        if (allowEmpty)
            return jsonContains(j, key, true) && j[key].is_array();
        return jsonContainsArray(j, key, false) && !j[key].empty();
    }

    bool jsonContainsObject(const json &j, const std::string &key, bool allowEmpty)
    {
        if (allowEmpty)
            return jsonContains(j, key, true) && j[key].is_object();
        return jsonContainsObject(j, key, true) && !j[key].empty();
    }

    bool jsonContainsString(const json &j, const std::string &key, bool allowEmpty)
    {
        if (allowEmpty)
            return jsonContains(j, key, true) && j[key].is_string();
        return jsonContainsString(j, key, true) && !j[key].empty();
    }

    json UnpackFile(const filesystem::path &jsonFilePath)
    {
        auto fileStr = ReadFileToString(jsonFilePath);
        if (fileStr.empty())
        {
            dbgln("UnpackFile: Failed to read JSON file: " + jsonFilePath.string() + "\nReturning null JSON.");
            return json();
        }

        try
        {
            return json::parse(fileStr);
        }
        catch (const json::parse_error& e)
        {
            dbgln("UnpackFile: Failed to parse JSON file: " + jsonFilePath.string() + ": " + e.what() + "\nReturning null JSON.");
            return json();
        }

        return json();
    }

    bool wantsReplacement(const json &j)
    {
        if (j.is_string())
        {
            if (j.get<std::string>().substr(0, strlen(REPLACEMENT_KEYWORD)) == REPLACEMENT_KEYWORD)
                return true;
        }

        return false;
    }

    std::string GetReplacementValue(const json &j, const std::string &key)
    {
        // Get the characters after the replacement keyword
        return j[key].get<std::string>().substr(strlen(REPLACEMENT_KEYWORD));
    }

    std::string GetReplacementValue(const json &j)
    {
        // Get the characters after the replacement keyword
        return j.get<std::string>().substr(strlen(REPLACEMENT_KEYWORD));
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

    json ReplaceKeyValueWithFile(json &j, const std::string &key, const filesystem::path &filePath)
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