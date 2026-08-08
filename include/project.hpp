#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "elements.hpp"

using json = nlohmann::json;

namespace lapCore
{
    enum class SyncProjectRuntimeMode
    {
        NONE,
        UPDATE_ON_SCENE,
        CONSTANT
    };

    struct IProjectElementData
    {
        virtual ~IProjectElementData() = default;
        virtual entt::id_type GetTypeID() const = 0;
        virtual std::string GetTypeName() const = 0;
        virtual void* GetDataPtr() = 0;
        virtual const void* GetDataPtr() const = 0;
        virtual std::unique_ptr<IProjectElementData> Clone() const = 0;
        SyncProjectRuntimeMode synced = SyncProjectRuntimeMode::NONE;

        json sourceJson;
    };

    template <typename T>
    struct ProjectElementData : IProjectElementData
    {
        T data;
        std::string type;

        std::unique_ptr<IProjectElementData> Clone() const override
        {
            return std::make_unique<ProjectElementData<T>>(*this);
        }

        entt::id_type GetTypeID() const override
        {
            return entt::type_hash<T>::value();
        }

        std::string GetTypeName() const override
        {
            return type;
        }

        void* GetDataPtr() override
        {
            return &data;
        }

        const void* GetDataPtr() const override
        {
            return &data;
        }
    };

    struct ProjectObjectData
    {
        std::string name;
        std::string parent;
        int child_index;

        std::vector<std::unique_ptr<IProjectElementData>> elements;

        ProjectObjectData() = default;
        ProjectObjectData(const ProjectObjectData& other)
        {
            name = other.name;
            parent = other.parent;
            child_index = other.child_index;
            for (const auto& element : other.elements)
            {
                if (element)
                    elements.push_back(element->Clone());
            }
        }

        ProjectObjectData& operator=(const ProjectObjectData& other)
        {
            if (this == &other)
                return *this;

            name = other.name;
            parent = other.parent;
            child_index = other.child_index;
            elements.clear();
            for (const auto& element : other.elements)
                elements.push_back(element->Clone());

            return *this;
        }

        ProjectObjectData(ProjectObjectData&&) = default;
        ProjectObjectData& operator=(ProjectObjectData&&) = default;
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

        std::unordered_map<std::string, std::string> data; // for any additional data, such as texture settings or shader uniform defaults
    };

    /*
    TODO:
    Prefabs is a tricky spot for handling Scene packing as JSON,
    Prefabs inside the instances key of a project JSON has the ability to use a string to point towards that prefab
    AND.
    Has the ability to essentially make a new prefab of its own, using the data of that already existing prefab, not exactly overwriting it, just using it as a basis and making its own changes
    */

    struct SceneInstancesData
    {
        std::vector<ProjectObjectData> objects;
        std::vector<ProjectObjectData> prefabs;
    };

    struct ProjectSceneData
    {
        std::string name;
        std::vector<ProjectSystemData> systems;
        SceneInstancesData instances;

        void Clear();
    };

    struct Project
    {
        Project() { RegisterDefaultElements(); }

        std::string name;
        std::string version;
        std::string path; // if there is any

        size_t main_scene_index = -1;

        std::vector<ProjectAssetData> assets;
        std::vector<ProjectSceneData> scenes;
        std::vector<ProjectObjectData> prefabs;

        // TODO: make these functions
        std::string PackAsString() const;
        void PackAsFiles(const std::string &folderPath) const;

        void RegisterDefaultElements();
    };
    
    Project UnpackProject(std::string projectJsonString);
}
