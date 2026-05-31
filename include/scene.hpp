#ifndef SCENE_HPP
#define SCENE_HPP

#include "system.hpp"
#include "project.hpp"

#include <iostream>

namespace lapCore
{
    struct World;

    using Object = entt::entity;

    struct ObjectInfo
    {
        entt::id_type id;
        Object object;
    };

    struct ObjectEntry
    {
        ObjectInfo info;
        ObjectInfo parent;
        std::map<unsigned int, ObjectInfo> children;
        int childIndex = -1;
        bool fromPrefab = false;
    };

    struct HierarchyEntry {
        entt::id_type id;
        entt::id_type parentID;
        std::map<unsigned int, entt::id_type> childrenIDs;
    };

    struct ObjectContainer
    {
        ~ObjectContainer() { ClearObjects(); }

        entt::registry objects;

        std::unordered_map<entt::id_type, ObjectEntry> objectMap;

        std::unordered_map<entt::id_type, std::string> lookup;
        std::vector<HierarchyEntry> hierarchy;

        Object AddObject(entt::hashed_string name, entt::hashed_string parent, int childIndex, bool fromPrefab = false);
        void RemoveObject(entt::id_type id);
        void RemoveObject(Object object);
        void RemoveObject(ObjectEntry entry);

        Object* FindObject(entt::id_type id);

        ObjectEntry* FindEntry(entt::id_type id);
        ObjectEntry* FindEntry(Object object);

        void AddObjectsFromProjectData(std::vector<ProjectObjectData> objects);

        void AddElement(entt::hashed_string objectName, entt::id_type elementType, void* elementData);
        void AddElement(Object object, entt::id_type elementType, void* elementData);
        void RemoveElement(entt::id_type id, entt::id_type elementType);
        void RemoveElement(Object object, entt::id_type elementType);
        void* FindElement(entt::id_type id, entt::id_type elementType);
        void* FindElement(Object object, entt::id_type elementType);

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

        void ClearObjects();
    };

    struct Scene : ObjectContainer
    {
        Scene(World* world, const std::string& name) : world(world), name(name) {}
        ~Scene() { Clear(); }

        std::map<int, std::unique_ptr<System>> systems;

        std::string name;
        World *world;

        template <typename T, typename... Args>
        void AddSystem(int order, Args&&... args)
        {
            if (order == -1) order = systems.rbegin()->first + 1;
            auto sys = std::make_unique<T>(this, order, std::forward<Args>(args)...);
            if constexpr (requires(T& t, entt::registry& r) { t.Connect(r); })
                sys->Connect(objects);

            systems[order] = std::move(sys);
        }

        template <typename T>
        T* GetSystem() const
        {
            for (const auto& [order, systemPtr] : systems)
            {
                if (T* foundSystem = dynamic_cast<T*>(systemPtr.get()))
                {
                    return foundSystem;
                }
            }

            dbgln("Error: System of type " + std::string(typeid(T).name()) + " not found!", LogType::ERROR);
            return nullptr;
        }

        void Update(float delta, RenderTexture2D &target);
        void Clear();
    };
}

#endif