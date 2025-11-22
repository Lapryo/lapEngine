#ifndef SCENE_HPP
#define SCENE_HPP

#include "systems.hpp"
#include "resource_manager.hpp"

#include <iostream>

namespace lapCore
{
    const unsigned int LOGICAL_RESOLUTION_REFERENCE = 800;

    using Object = entt::entity;

    struct AssetLoadRequest
    {
        std::string name;
        std::string type;
        std::string path;
    };

    struct ObjectEntry
    {
        Object object;
        Object parent;
        std::vector<Object> children;
    };

    struct Scene
    {
        std::string name;
        entt::registry objects;
        std::unordered_map<std::string, ObjectEntry> objectMap;

        std::vector<std::unique_ptr<System>> systems;

        std::vector<AssetLoadRequest> queuedAssets;
        ResourceManager resources;

        Vector2 logicalWindowPos;
        Vector2 logicalResolution;
        double resolutionScale = 1;

        void QueueAsset(const std::string &name, const std::string &type, const std::string &path);
        void QueueAsset(const AssetLoadRequest &asset);
        void LoadQueuedAssets();

        void Update(float deltaTime, RenderTexture2D &target);

        template <typename SystemType, typename... SystemArgs>
        void AddSystem(unsigned int order, SystemArgs &&...args)
        {
            auto sys = std::make_unique<SystemType>(this, order, std::forward<SystemArgs>(args)...);
            if constexpr (requires(SystemType &t, entt::registry &r) { t.Connect(r); })
                sys->Connect(objects);

            if (order > systems.size())
                systems.resize(order + 1);

            systems[order] = std::move(sys);
        }

        template <typename T>
        T *GetSystem() const
        {
            for (const auto &systemPtr : systems)
            {
                if (T *foundSystem = dynamic_cast<T *>(systemPtr.get()))
                {
                    return foundSystem;
                }
            }

            std::cerr << "Error: System of type " << typeid(T).name() << " not found!" << std::endl;
            return nullptr;
        }

        Object AddObject(const std::string &name, const std::string &parent, int childIndex);
        void RemoveObject(Object object);
        Object FindObject(const std::string &name);

        std::string GetObjectName(Object object);

        std::vector<Object> GetChildren(Object object);
        Object FindChild(Object object, const std::string &name);

        template <typename Element, typename... ElementArgs>
        Element AddElement(Object object, ElementArgs &&...args)
        {
            return objects.emplace<Element>(object, std::forward<ElementArgs>(args)...);
        }

        template <typename Element>
        void RemoveElement(Object object)
        {
            objects.remove<Element>(object);
        }

        template <typename Element>
        Element *FindElement(Object object)
        {
            return objects.try_get<Element>(object);
        }

        template <typename Element>
        entt::view<Element> GetElements()
        {
            return objects.view<Element>();
        }

        void Clear();
    };
}

#endif