#pragma once

#include "system.hpp"
#include "project.hpp"
#include "map.hpp"

#include <iostream>

using namespace lapCore;
using namespace ELEMENTS;
using namespace ELEMENTS::Other;

namespace lapCore
{
    using namespace EUTIL;
    using namespace Functions;
    using namespace Other;

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

    /*struct ObjectInfo
    {
        Object object;
        Object parent;
    };

    struct NewContainer
    {
        std::unordered_map<entt::id_type, tree<std::forward_list<ObjectInfo>>::iterator> objectLookup;
        tree<std::forward_list<ObjectInfo>> objectTree;

        
            Implement a queue system that works in layers, layer 1 is the children of the root node (the window),
            layer 2 is the children of the layer 1 nodes, and so on.

            In the ResolvePendingObjects function, we will determine what objects are in what layers based of their distance
            from the head node.
        

        Object AddObject(const std::string& name, Object parent = entt::null)
        {
            Object object = objects.create();

            entt::id_type id = HASH_ID(name.c_str());
            if (parent == entt::null)
            {
                std::forward_list<ObjectInfo> objectList;
                objectList.push_front({ object, entt::null });
                objectLookup[id] = objectTree.insert_after(objectTree.head->last_child, std::move(objectList));

                return object;
            }

            if (objectLookup.contains(id))
            {
                // That means the iterator exists and we can just add the object to the linked list
                auto node = objectLookup[id].node;

                bool matchingParent = false;
                for (const auto& obj : node->parent->data)
                {
                    if (obj.object == parent)
                    {
                        matchingParent = true;
                        break;
                    }
                }

                if (matchingParent)
                {
                    return object;
                }

                // The object parent may soon exist, so we need to add it to a queue to get resolved later

                node->data.push_front({ object, parent });
            }
            else
            {
                // The object doesn't exist, so we create it
                // The problem is, the parent may soon exist, so we need to add it to a queue to be resolved later

            }

            return object;
        }

        void ResolvePendingObjects();

    private:
        entt::registry objects;
        std::vector<ObjectInfo> objectQueue;
    };*/

    struct ObjectContainer
    {
        ~ObjectContainer() { ClearObjects(); }

        entt::registry objects;

        std::unordered_map<entt::id_type, ObjectEntry> objectMap;

        std::unordered_map<entt::id_type, std::string> lookup;

        ObjectInfo AddObject(entt::hashed_string name, entt::hashed_string parent, int childIndex, bool fromPrefab = false);
        void RemoveObject(entt::id_type id);
        void RemoveObject(Object object);
        void RemoveObject(ObjectEntry entry);

        Object* FindObject(entt::id_type id);

        ObjectEntry* FindEntry(entt::id_type id);
        ObjectEntry* FindEntry(Object object);

        ObjectInfo CloneObject(Object object, std::string newName = "");
        ObjectInfo CloneObjectFromContainer(ObjectContainer &container, Object object, entt::hashed_string newName = "", entt::hashed_string newParent = "");
        ObjectInfo CloneHierarchyFromContainer(ObjectContainer &container, Object object, entt::hashed_string newName = "", entt::hashed_string newParent = "");

        void AddObjectsFromProjectData(std::vector<ProjectObjectData> objects);
        void AddObjectsFromPrefabProjectData(ObjectContainer& prefabContainer, const std::vector<std::pair<std::string, ProjectObjectData>>& prefabs);

        ObjectInfo AddObjectFromObjectData(ProjectObjectData objectData);

        void AddElement(entt::hashed_string objectName, entt::id_type elementType, void* elementData);
        void AddElement(Object object, entt::id_type elementType, void* elementData);
        void RemoveElement(entt::id_type id, entt::id_type elementType);
        void RemoveElement(Object object, entt::id_type elementType);
        void* FindElement(entt::id_type id, entt::id_type elementType);
        void* FindElement(Object object, entt::id_type elementType);

        template <typename Element, typename... ElementArgs>
        Element& AddElement(Object object, ElementArgs &&...args)
        {
            return objects.emplace<Element>(object, std::forward<ElementArgs>(args)...);
        }

        template <typename Element, typename... ElementArgs>
        Element* AddElement(entt::id_type objectID, ElementArgs &&...args)
        {
            auto obj = FindObject(objectID);
            if (!obj) return nullptr;
            auto& element = AddElement<Element>(*obj, std::forward<ElementArgs>(args)...);
            
            return &element;
        }

        template <typename Element>
        void RemoveElement(Object object)
        {
            objects.remove<Element>(object);
        }

        template <typename Element>
        bool RemoveElement(entt::id_type objectID)
        {
            auto obj = FindObject(objectID);
            if (!obj) return false;
            RemoveElement<Element>(*obj);
            return true;
        }

        template <typename Element>
        Element *FindElement(Object object)
        {
            return objects.try_get<Element>(object);
        }

        template <typename Element>
        Element* FindElement(entt::id_type objectID)
        {
            auto obj = FindObject(objectID);
            if (!obj) return nullptr;
            return FindElement<Element>(*obj);
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

        Color backgroundColor = WHITE;

        void LoadMapObjects(Map& map);

        lapCore::ObjectInfo *AddPrefab(entt::id_type prefabName, std::string newName = "");
        lapCore::ObjectInfo AddPrefab(Object prefab, std::string newName = "");

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

        template <typename... Args>
        void SpawnScript(entt::id_type objectID, entt::hashed_string eventName, void (*func)(Scene*, entt::id_type, entt::id_type, Args...))
        {
            auto object = FindObject(objectID);
            if (!object) return;

            auto* script = FindElement<Script>(*object);
            if (script)
            {
                script->onUpdateFunctions.push_back(eventName.value());
            }
            else
            {
                Script newScript;
                newScript.onUpdateFunctions.push_back(eventName.value());

                AddElement<Script>(*object, newScript);
            }

            ConnectECSEvent<Args...>(objectID, eventName, func);
        }

        void Clear();
    };
}