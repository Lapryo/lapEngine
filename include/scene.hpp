#pragma once

#include "system.hpp"
#include "project/project.hpp"

#include <iostream>

using namespace lapCore;
using namespace ELEMENTS;
using namespace ELEMENTS::Other;
using namespace entt;
using namespace std;

namespace lapCore
{
    using namespace EUTIL;
    using namespace Functions;
    using namespace Other;

    struct World;

    using Object = entity;

    struct ObjectData
    {
        id_type id;
        Object parent;
        map<unsigned int, Object> children;
        int index = -1;
    };

    struct Scene
    {
        Scene(World* world);
        Scene(World* world, const ProjectSceneData& data);

        Color backgroundColor = BLACK;

        Object AddObject(const ProjectObjectData& data);
        Object AddPrefabObject(const ProjectInstancedPrefabData& data);

        Object CloneObject(Object object, ObjectData newData);
        Object CloneObjectFromRegistry(Object otherObject, const registry &otherRegistry, ObjectData newData);
        Object AddPrefab(Object prefabObject, ObjectData newData);

        // Adds an object to the root object of the scene
        Object AddObject(id_type id, int index = -1, ShiftMode shiftMode = ShiftMode::CLUMPING);

        // Adds an object to the scene with a specified parent and index
        Object AddObject(id_type id, Object parent, int index = -1, ShiftMode shiftMode = ShiftMode::CLUMPING);

        // Gets all objects with a specific id
        vector<Object> FindObjects(id_type id);

        // Gets the first object with a specific id (the one closest to the root)
        Object FindFirstObject(id_type id);

        // Gets all children of a specific parent
        vector<Object> GetChildren(Object parent);

        // Gets the child at a specific index
        Object FindChild(Object parent, unsigned int index = -1);

        // Gets all children of a specific parent with the same ids
        vector<Object> FindChildren(Object parent, id_type id);

        // Finds the first child with a specific id under a specific parent (the one with the lowest index)
        Object FindFirstChild(Object parent, id_type id);

        // Removes an object and all of its descendants from the scene
        bool RemoveObject(Object object, ShiftMode shiftMode = ShiftMode::CLUMPING);

        // Reparents an object and all of its descendants to a new parent
        bool ReparentObject(Object object, Object newParent, ShiftMode startShiftMode = ShiftMode::CLUMPING, ShiftMode endShiftMode = ShiftMode::CLUMPING);

        // Gets the object data for a specific object
        ObjectData* GetObject(Object object);

        void Update(float delta, RenderTexture2D &target);

        template <typename System, typename... Args>
        System* AddSystem(int order, Args&&... args)
        {
            if (order == -1) order = systems.end()->first + 1;
            auto sys = make_unique<System>(this, order, forward<Args>(args)...);
            auto ptr = sys.get();
            if constexpr (requires(System& t, registry& r) { t.Connect(r); })
                sys->Connect(objects);

            systems[order] = std::move(sys);
            return ptr;
        }

        template <typename System>
        System* FindSystem()
        {
            for (const auto& [order, systemPtr] : systems)
            {
                if (System* foundSystem = dynamic_cast<System*>(systemPtr.get()))
                {
                    return foundSystem;
                }
            }

            return nullptr;
        }

        template <typename System>
        bool RemoveSystem()
        {
            auto sys = FindSystem<System>();
            if (sys)
            {
                systems.erase(find_if(systems.begin(), systems.end(), [sys](const auto& pair) { return pair.second.get() == sys; }));
                return true;
            }
            return false;
        }

        template <typename Element, typename... ElementArgs>
        Element& AddElement(Object object, ElementArgs &&...args)
        {
            return objects.emplace<Element>(object, std::forward<ElementArgs>(args)...);
        }

        template <typename Element>
        Element* CloneElement(Object srcObject, Object dstObject)
        {
            auto element = FindElement<Element>(srcObject);
            if (element) return &AddElement<Element>(dstObject, &element);
            return nullptr;
        }

        template <typename Element>
        void RemoveElement(Object object)
        {
            objects.remove<Element>(object);
        }

        template <typename Element>
        Element* FindElement(Object object)
        {
            return objects.try_get<Element>(object);
        }

        World* world;
        id_type id;

        map<int, unique_ptr<System>> systems;
        registry objects;

        Object root;
    };
}