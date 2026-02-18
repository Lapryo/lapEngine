#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "elements.hpp"

namespace lapCore
{
    enum class ProjectDataType
    {
        SCENE,
        OBJECT,
        SYSTEM,
        COMPONENT
    };

    struct ProjectElementData
    {
        std::string type;
        std::variant<
            std::monostate,
            Origin2D,
            Physics2D,
            Rotation2D,
            Frame,
            UIList,
            Sprite,
            Image,
            TextLabel,
            EventBus,
            UIButton,
            Cam2D,
            Attribute<std::any>,
            Script
        > data;
    };

    struct ProjectObjectData
    {
        std::string name;
        std::string parent;
        int child_index;

        std::vector<ProjectElementData> elements;
    };

    struct ProjectSystemData
    {
        std::string type;
        unsigned int order;
    };

    struct ProjectAssetData
    {
        std::string name;
        std::string path;
        std::string type;
    };

    struct ProjectSceneData
    {
        std::string name;
        std::vector<ProjectSystemData> systems;
        std::vector<ProjectObjectData> instances;
    };

    struct Project
    {
        std::string name;
        std::string version;
        std::string path; // if there is any

        size_t main_scene_index = -1;

        std::vector<ProjectAssetData> assets;
        std::vector<ProjectSceneData> scenes;
        std::vector<ProjectObjectData> prefabs;

        std::string Pack() const;
    };
    
    Project UnpackProject(std::string projectJsonString);
}