#pragma once

#include <raylib.h>
#include <entt/entt.hpp>
#include "project/unpacker.hpp"

namespace lapCore
{
    inline bool defaultStructsRegistered = false;
    bool RegisterDefaultStructs();

    struct ProjectAssetData
    {
        string name, type;
        filesystem::path path;

        unordered_map<string, string> data;
    };

    struct ProjectSystemData
    {
        string type;
        int order;
    };

    struct IProjectElementData
    {
        virtual ~IProjectElementData() = default;

        virtual entt::id_type GetTypeID() const = 0;
        virtual string GetTypeName() const = 0;

        virtual void* GetRawData() = 0;
        virtual const void* GetRawData() const = 0;

        virtual unique_ptr<IProjectElementData> Clone() const = 0;

        json sourceJson;
    };

    template <typename T>
    struct ProjectElementData : IProjectElementData
    {
        string type;
        T data;

        entt::id_type GetTypeID() const override
        {
            return entt::type_hash<T>::value();
        }

        string GetTypeName() const override
        {
            return type;
        }

        void* GetRawData() override
        {
            return &data;
        }

        const void* GetRawData() const override
        {
            return &data;
        }

        unique_ptr<IProjectElementData> Clone() const override
        {
            return make_unique<ProjectElementData<T>>(*this);
        }
    };

    struct ProjectObjectData
    {
        string name, parent;
        int index;

        vector<unique_ptr<IProjectElementData>> elements;

        ProjectObjectData() = default;

        ProjectObjectData(const ProjectObjectData& other);
        ProjectObjectData& operator=(const ProjectObjectData& other);

        ProjectObjectData(ProjectObjectData&&) = default;
        ProjectObjectData& operator=(ProjectObjectData&&) = default;
    };

    struct ProjectInstancedPrefabData
    {
        string prefabName;
        ProjectObjectData objectData;

        vector<ProjectInstancedPrefabData> children;
    };

    struct ProjectSceneData
    {
        string name;
        vector<ProjectSystemData> systems;
        vector<ProjectObjectData> objects;

        vector<ProjectInstancedPrefabData> instancedPrefabs;

        Color backgroundColor = WHITE;
    };

    struct Project
    {
        string name, version;
        filesystem::path path;
        
        vector<ProjectAssetData> assets;
        vector<ProjectSceneData> scenes;
        vector<ProjectObjectData> prefabs;
    };

    Project LoadProject(const filesystem::path& path);
}