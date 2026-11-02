#pragma once

#include "system.hpp"
#include "project/project.hpp"
#include "map.hpp"
#include "tree.hpp"

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

    struct ObjectMetadata
    {
        Object* object;
        id_type* type;

        int* child_index;
        bool* fromPrefab;
    };

    /*



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
    */

    struct ObjectContainer
    {
        id_type id;
        registry objects;

        // Adders
        Object AddObject(id_type id);
        Object AddObject(const ProjectObjectData& data);
        Object AddObject(Object parent, id_type id);
        Object AddChild(Object parent, id_type id);

        Object CloneObject(Object object, id_type newID);
        Object CloneObject(Object object, id_type newID, Object newParent);

        // Getters
        vector<Object> FindObjects(id_type id);
        Object FindFirstObject(id_type id); // Gets the object closest to the trees root node and with the lowest child_index
        KVNode<ObjectMetadata, Object, id_type>* GetNode(Object object);

        vector<Object> GetChildren(Object object);
        vector<Object> FindChildren(Object parent, id_type id);
        Object FindFirstChild(Object parent, id_type id);

        bool RemoveObject(Object object);
        SuccessType RemoveAllObjects(id_type id);
        bool ReparentObject(Object object, Object newParent);
        
        ObjectMetadata* SetObjectData(Object object, ObjectMetadata data);
        ObjectMetadata* GetObjectData(Object object);

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
        Element *FindElement(Object object)
        {
            return objects.try_get<Element>(object);
        }

    private:
        KVTree<ObjectMetadata, Object, id_type> tree;
    };

    struct Scene : ObjectContainer
    {
        Scene(World* world);
        Scene(World* world, const ProjectSceneData& data);

        World* world;

        template <typename System, typename... Args>
        System* AddSystem(int order, Args&&... args)
        {
            if (order == -1) order = systems.rbegin()->first + 1;
            auto sys = std::make_unique<System>(this, order, std::forward<Args>(args)...);
            if constexpr (requires(System& t, entt::registry& r) { t.Connect(r); })
                sys->Connect(objects);

            systems[order] = std::move(sys);
            return systems[order].get();
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
                systems.erase(std::find_if(systems.begin(), systems.end(), [sys](const auto& pair) { return pair.second.get() == sys; }));
                return true;
            }
            return false;
        }

    private:
        map<int, unique_ptr<System>> systems;
    };

    /*
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
    */
}