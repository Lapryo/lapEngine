#ifndef SCENE_HPP
#define SCENE_HPP

#include "system.hpp"
#include "resource_manager.hpp"

#include <iostream>

namespace lapCore
{
    struct World;

    using Object = entt::entity;

    struct ObjectInfo
    {
        std::string name;
        Object object;
    };

    struct ObjectEntry
    {
        ObjectInfo info;
        ObjectInfo parent;
        std::vector<ObjectInfo> children;
        int childIndex = -1;
    };

    struct Scene
    {
        Scene() {}
        Scene(World *world, const std::string &name) : world(world), name(name) {}

        World *world;

        std::string name;

        entt::registry objects;
        std::unordered_map<std::string, ObjectEntry> objectMap;

        entt::registry prefabs;
        std::unordered_map<std::string, ObjectEntry> prefabMap;

        std::vector<std::unique_ptr<System>> systems;

        void Update(float deltaTime, rl::RenderTexture2D &target);

        template <typename SystemType, typename... SystemArgs>
        void AddSystem(int order, SystemArgs &&...args)
        {
            if (order < 0) order = systems.size() + 1;

            auto sys = std::make_unique<SystemType>(this, order, std::forward<SystemArgs>(args)...);
            if constexpr (requires(SystemType &t, entt::registry &r) { t.Connect(r); })
                sys->Connect(objects);

            if (order > systems.size())
                systems.resize(order);

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
        ObjectEntry FindObject(const std::string &name);

        Object AddPrefab(const std::string &name, const std::string &parent, int childIndex);
        ObjectEntry FindPrefab(const std::string &name);

        Object AddObjectFromPrefab(const std::string &prefabName, const std::string &newName);

        std::string GetObjectName(Object object);

        std::vector<ObjectInfo> GetChildren(Object object);
        Object FindChild(Object object, const std::string &name);

        void SetParent(const std::string &name, Object parent);
        ObjectInfo GetParent(const std::string &name);

        template <typename Element, typename... ElementArgs>
        Element AddElement(entt::registry &registry, Object object, ElementArgs &&...args)
        {
            return registry.emplace<Element>(object, std::forward<ElementArgs>(args)...);
        }

        template <typename Element>
        void RemoveElement(entt::registry &registry, Object object)
        {
            registry.remove<Element>(object);
        }

        template <typename Element>
        Element *FindElement(entt::registry &registry, Object object)
        {
            return registry.try_get<Element>(object);
        }

        template <typename Element>
        entt::view<Element> GetElements(entt::registry &registry)
        {
            return registry.view<Element>();
        }

        void Clear();
    };
}

#endif