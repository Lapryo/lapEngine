#include "scene.hpp"
#include "world.hpp"
#include "systems.hpp"
#include "event.hpp"

#include "reflection.hpp"

#include <iostream>

using namespace lapCore;
using namespace EUTIL::Functions;

namespace lapCore
{
    Object Scene::AddObject(const ProjectObjectData& data)
    {
        // TODO: By default, we'll get the closest index, or the one closest to the root, but maybe add a queue in the future that'll wait for the parents to exist (with timeouts of course)
        auto parent = (data.parent.empty()) ? this->root : FindFirstObject(HASH_ID(data.parent.c_str()));
        auto object = AddObject(HASH_ID(data.name.c_str()), parent, data.index);

        for (const auto& elementData : data.elements)
        {
            auto entry = Reflection::TryGet(elementData->GetTypeID());
            if (!entry) continue;
            entry->emplace(objects, object, elementData->GetRawData());
        }

        return object;
    }

    Object Scene::AddPrefabObject(ProjectInstancedPrefabData &data)
    {
        auto object = AddObject(data.objectData);
        for (auto &childData : data.children)
        {
            auto obj = AddPrefabObject(childData);
            childData.objectData.object = obj;
        }

        return object;
    }

    Object Scene::CloneObject(
        Object object,
        ObjectData newData
    )
    {
        return CloneObjectFromRegistry(
            object,
            objects,
            newData
        );
    }

    Object Scene::CloneObjectFromRegistry(
        Object otherObject,
        const registry& otherRegistry,
        ObjectData newData
    )
    {
        const auto* sourceData = otherRegistry.try_get<ObjectData>(otherObject);
        if (!sourceData)
            return entt::null;

        // Copy the children before adding anything to the registry.
        // This prevents same-registry cloning from modifying our iteration.
        auto children = sourceData->children;

        // Create the cloned object.
        Object object = AddObject(
            newData.id,
            newData.parent,
            newData.index
        );

        if (object == entt::null)
            return entt::null;

        // Copy all components without knowing their types.
        for (auto&& [id, storage] : otherRegistry.storage())
        {
            // ObjectData is managed by AddObject.
            if (id == entt::type_hash<ObjectData>::value())
                continue;

            if (!storage.contains(otherObject))
                continue;

            auto* destination = objects.storage(id);

            if (!destination)
            {
                // A matching storage must exist in the destination.
                std::cerr << "Missing destination storage: " << id << '\n';
                continue;
            }

            destination->push(object, storage.value(otherObject));
        }

        // Clone descendants recursively.
        for (const auto& [index, child] : children)
        {
            const auto* childData = otherRegistry.try_get<ObjectData>(child);
            if (!childData)
                continue;

            ObjectData data{};
            data.id = childData->id;
            data.parent = object;
            data.index = static_cast<int>(index);

            CloneObjectFromRegistry(child, otherRegistry, data);
        }

        return object;
    }

    Object Scene::AddPrefab(
        Object prefabObject,
        ObjectData newData
    )
    {
        if (!world || !world->prefabs)
            return null;

        return CloneObjectFromRegistry(
            prefabObject,
            world->prefabs->objects,
            newData
        );
    }

    Scene::Scene(World *world)
    {
        this->world = world;
        this->root = objects.create();

        ObjectData rootData;
        rootData.id = HASH_ID("__root__");
        rootData.parent = null;
        rootData.index = -1;

        AddElement<ObjectData>(this->root, rootData);
    }

    Scene::Scene(
        World* world,
        const ProjectSceneData& data,
        Scene::LoadMode mode
    )
    {
        std::cout << "Initializing scene.\n";
        this->world = world;
        this->id = HASH_ID(data.name.c_str());
        this->initialData = data;
        this->backgroundColor = data.backgroundColor;

        std::cout << "Creating root.\n";
        ObjectData rootData;
        rootData.id = HASH_ID("__root__");
        rootData.parent = null;
        rootData.index = -1;

        this->root = objects.create();
        AddElement<ObjectData>(this->root, rootData);

        std::cout << "Creating objects.\n";
        // Create all entities and attach their components.
        for (auto& objectData : initialData->objects)
        {
            auto obj = AddObject(objectData);
            objectData.object = obj;
        }

        std::cout << "Creating instanced prefabs.\n";
        for (auto& prefabData : initialData->instancedPrefabs)
        {
            auto obj = AddPrefabObject(prefabData);
            prefabData.objectData.object = obj;
        }

        std::cout << "Getting snapshot.\n";
        // Flatten all instantiated objects into ProjectSceneData.
        ProjectSceneData snapshot = toData();

        // Systems haven't been initialized, so retain
        // their definitions from the original project.
        std::cout << "Assigning initial data.\n";
        snapshot.systems = data.systems;

        initialData = std::move(snapshot);

        // Preserve entity handles and remove runtime components.
        std::cout << "Making the scene stale.\n";
        MakeStale(false);

        // MakeStale() currently captures runtimeData using toData(),
        // which also doesn't see any initialized systems yet.
        if (runtimeData)
            runtimeData->systems = data.systems;

        // Activate only the requested scene.
        if (mode == Scene::LoadMode::ACTIVE)
        {
            std::cout << "Restoring scene.\n";
            Restore(true);
        }
    }

    Scene::~Scene()
    {
        objects.clear();
        systems.clear();
        runtimeData.reset();
        initialData.reset();
    }

    void Scene::MakeStale(bool disableEvents, vector<Object> excludeList)
    {
        if (disableEvents)
        {
            for (const auto &object : objects.view<Object>())
            {
                bool found = false;
                for (const auto& excludedObject : excludeList)
                {
                    if (object == excludedObject)
                    {
                        found = true;
                        break;
                    }
                }

                if (found) continue;
                EventRegistry::SetActive(this, false, true);
            }
        }

        runtimeData = toData();
        systems.clear();

        // Remove components here
        for (auto&& [id, storage] : objects.storage())
        {
            if (id == entt::type_hash<ObjectData>::value())
                continue;

            storage.clear();
        }
    }

    void GatherAllPrefabObjectData(vector<ProjectObjectData> &list, const ProjectInstancedPrefabData &data)
    {
        list.push_back(data.objectData);
        for (const auto& childData : data.children)
        {
            GatherAllPrefabObjectData(list, childData);
        }
    }

    void Scene::RestoreObject(const ProjectObjectData& data)
    {
        if (!data.object)
            return;

        Object object = *data.object;

        if (!objects.valid(object))
            return;

        for (const auto& elementData : data.elements)
        {
            if (!elementData)
                continue;

            auto entry = Reflection::TryGet(elementData->GetTypeID());
            if (!entry)
                continue;

            entry->emplace(
                objects,
                object,
                elementData->GetRawData()
            );
        }
    }

    void Scene::Restore(bool useRuntimeData)
    {
        const ProjectSceneData* sourceData = nullptr;

        if (useRuntimeData && runtimeData)
            sourceData = &*runtimeData;
        else if (initialData)
            sourceData = &*initialData;

        if (!sourceData)
            return;

        // Normal scene objects
        for (const auto& data : sourceData->objects)
        {
            RestoreObject(data);
        }

        /*
            Because we are restoring from an already existing scene, we don't need to go through the
            instanced prefabs, because they were already instanciated! (i think thats how you spell it)
        */

        // Straightforward
        for (const auto& systemData : sourceData->systems)
        {
            if (systemData.type == "gui")
            {
                AddSystem<GUISystem>(systemData.order);
            }
            else if (systemData.type == "physics")
            {
                auto physics = AddSystem<PhysicsSystem>(systemData.order);
                physics->RegisterBodies();
            }
            else if (systemData.type == "render")
            {
                AddSystem<RenderSystem>(systemData.order);
            }
            else if (systemData.type == "script")
            {
                AddSystem<ScriptSystem>(systemData.order);
            }
            else if (systemData.type == "input")
            {
                AddSystem<InputSystem>(systemData.order);
            }
            else if (systemData.type == "sound")
            {
                AddSystem<SoundSystem>(systemData.order);
            }
            else
            {
                dbgln("Unknown system type: " + systemData.type, LogType::WARNING);
            }
        }

        // Retoggle all events
        EventRegistry::SetActive(this, true, true);
        runtimeData.reset();
    }

    Object Scene::AddObject(id_type id, int index, ShiftMode shiftMode)
    {
        return AddObject(id, root, index, shiftMode);
    }

    Object Scene::AddObject(
        id_type id,
        Object parent,
        int index,
        ShiftMode shiftMode
    )
    {
        auto* parentData = GetObject(parent);

        if (!parentData || index < -1)
            return null;

        unsigned int newIndex;

        if (index == -1)
        {
            if (parentData->children.empty())
            {
                newIndex = 0;
            }
            else
            {
                auto lastIndex = parentData->children.rbegin()->first;

                if (lastIndex == UINT_MAX)
                    return null;

                newIndex = lastIndex + 1;
            }
        }
        else
        {
            newIndex = static_cast<unsigned int>(index);
        }

        auto object = objects.create();

        ObjectData data{};
        data.id = id;
        data.parent = parent;
        data.index = newIndex;

        AddElement<ObjectData>(object, data);

        // Reacquire after possible component storage reallocation.
        parentData = GetObject(parent);

        if (index == -1)
        {
            // Appending does not affect existing indices.
            parentData->children.emplace_hint(
                parentData->children.end(),
                newIndex,
                object
            );
        }
        else
        {
            ShiftMap(
                parentData->children,
                shiftMode,
                newIndex,
                object,
                true
            );

            // Only needed when an existing child may have shifted.
            for (const auto& [childIndex, child] : parentData->children)
            {
                GetObject(child)->index = childIndex;
            }
        }

        return object;
    }

    vector<Object> Scene::FindObjects(id_type id)
    {
        auto view = objects.view<ObjectData>();
        vector<Object> result;

        for (auto entity : view)
        {
            auto& data = view.get<ObjectData>(entity);
            if (data.id == id)
            {
                result.push_back(entity);
            }
        }

        return result;
    }

    Object Scene::FindFirstObject(id_type id)
    {
        auto view = FindObjects(id);

        // Get the one closest to the root
        Object closestObject = null;
        int closestDepth = INT_MAX;

        for (const auto& object : view)
        {
            int depth = 0;
            auto current = object;

            while (current != root && current != null)
            {
                auto* data = GetObject(current);
                if (!data) break;
                current = data->parent;
                depth++;
            }

            if (depth < closestDepth)
            {
                closestDepth = depth;
                closestObject = object;
            }
        }

        return closestObject;
    }

    vector<Object> Scene::GetChildren(Object parent)
    {
        auto parentData = GetObject(parent);
        if (!parentData)
            return {};

        auto result = vector<Object>();
        for (const auto& [index, child] : parentData->children)
        {
            result.push_back(child);
        }

        return result;
    }

    Object Scene::FindChild(Object parent, unsigned int index)
    {
        auto parentData = GetObject(parent);
        if (!parentData)
            return null;

        auto it = parentData->children.find(index);
        if (it != parentData->children.end())
            return it->second;

        return null;
    }

    vector<Object> Scene::FindChildren(Object parent, id_type id)
    {
        auto parentData = GetObject(parent);
        if (!parentData)
            return {};

        vector<Object> result;
        for (const auto& [index, child] : parentData->children)
        {
            auto* childData = GetObject(child);
            if (childData && childData->id == id)
            {
                result.push_back(child);
            }
        }

        return result;
    }

    Object Scene::FindFirstChild(Object parent, id_type id)
    {
        auto parentData = GetObject(parent);
        if (!parentData)
            return null;

        for (const auto& [index, child] : parentData->children)
        {
            auto* childData = GetObject(child);
            if (childData && childData->id == id)
            {
                return child;
            }
        }

        return null;
    }

    bool Scene::RemoveObject(Object object, ShiftMode shiftMode)
    {
        // Remove all descendants first
        auto* data = GetObject(object);
        if (!data)
            return false;

        for (const auto& [index, child] : data->children)
        {
            RemoveObject(child);
        }

        // Remove from the parent's children map and shift the indices
        auto* parentData = GetObject(data->parent);
        if (parentData)
        {
            ShiftMap(
                parentData->children,
                shiftMode,
                data->index,
                Object{},
                false
            );
        }

        // Finally, destroy the object
        objects.destroy(object);
        return true;
    }

    bool Scene::ReparentObject(Object object, Object newParent, ShiftMode startShiftMode, ShiftMode endShiftMode)
    {
        auto* data = GetObject(object);
        if (!data)
            return false;

        auto* oldParentData = GetObject(data->parent);
        auto* newParentData = GetObject(newParent);

        if (!newParentData)
            return false;

        // Remove from the old parent's children map and shift the indices
        if (oldParentData)
        {
            ShiftMap(
                oldParentData->children,
                startShiftMode,
                data->index,
                Object{},
                false
            );
        }

        // Update the object's parent and index
        data->parent = newParent;
        data->index = static_cast<unsigned int>(newParentData->children.size());

        // Add to the new parent's children map and shift the indices
        ShiftMap(
            newParentData->children,
            endShiftMode,
            data->index,
            object,
            true
        );

        // Synchronize child indices for the new parent
        for (const auto& [childIndex, child] : newParentData->children)
        {
            GetObject(child)->index = childIndex;
        }

        return true;
    }

    ObjectData *Scene::GetObject(Object object)
    {
        return FindElement<ObjectData>(object);
    }

    void Scene::Update(float deltaTime, RenderTexture2D &target)
    {
        /// Draw to render texture first
        BeginTextureMode(target);
        ClearBackground(this->backgroundColor);

        /*static unordered_map<string, double> timers;
        static double timerforseconds = GetTime();
        static int frameCounter = 0;
        static int counter = 0;*/

        for (auto &[order, system] : systems)
        {
            if (!system || !system->active)
                continue;

            //auto now = GetTime();
            system->Update(deltaTime, objects);
            //auto time = GetTime() - now;

            //timers[system->GetName()] += time;
        }

        /*if (GetTime() - timerforseconds >= 1)
        {
            counter++;
            cout << "[AVERAGES FROM SECOND " << counter << "]\n";
            for (const auto& [name, time] : timers)
            {
                cout << name << ": " << (time / frameCounter) * 1000.0 << '\n';
            }

            timers.clear();
            frameCounter = 0;
            timerforseconds = GetTime();
        }

        frameCounter++;*/

        EndTextureMode();

        // Draw to the screen with the render texture afterwards
        BeginDrawing();
        ClearBackground(BLACK);

        // Makes the window letterboxed/pillarboxed to maintain the aspect ratio of the logical resolution
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        float screenAspect = (float)screenW / screenH;
        float targetAspect = (float)world->window.logical_resolution.x / world->window.logical_resolution.y;

        int drawWidth, drawHeight;
        int offsetX, offsetY;

        if (screenAspect > targetAspect)
        {
            // window is wider than logical
            drawHeight = screenH;
            drawWidth = (int)(screenH * targetAspect);
            offsetX = (screenW - drawWidth) / 2;
            offsetY = 0;
        }
        else
        {
            // window is taller than logical
            drawWidth = screenW;
            drawHeight = (int)(screenW / targetAspect);
            offsetX = 0;
            offsetY = (screenH - drawHeight) / 2;
        }

        // Draw the render texture to the screen, scaling it
        DrawTexturePro(
            target.texture,
            {0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height}, // source rect (flip y)
            {(float)offsetX, (float)offsetY, (float)drawWidth, (float)drawHeight},    // dest rect
            {0.0f, 0.0f},                                                             // origin
            0.0f,                                                                     // rotation
            WHITE);

        // TODO: Here we can draw a custom debug menu with system times, etc. if needed for developers
        // This option can be toggleable and is checked via the App class

        EndDrawing();
    }

    ProjectSceneData Scene::toData()
    {
        ProjectSceneData data;

        data.name = GET_FORMER_HASHED(this->id);
        data.backgroundColor = this->backgroundColor;

        // Serialize systems.

        std::cout << "Gathering systems.\n";
        for (const auto& [order, system] : systems)
        {
            if (!system)
                continue;

            ProjectSystemData systemData;
            systemData.order = system->order;
            systemData.type = system->GetName();

            data.systems.push_back(std::move(systemData));
        }

        // Serialize objects.
        std::cout << "Gathering objects.\n";
        for (auto [object, objectData] : objects.view<ObjectData>().each())
        {
            if (object == root)
                continue;

            ProjectObjectData projectObjData;

            projectObjData.object = object;
            projectObjData.name = GET_FORMER_HASHED(objectData.id);
            projectObjData.index = objectData.index;

            std::cout << "Making object " << projectObjData.name << '\n';

            if (objectData.parent == root)
            {
                projectObjData.parent = "";
            }
            else
            {
                std::cout << "Getting parent information.\n";
                if (objectData.parent == null) continue;
                auto* parentData = GetObject(objectData.parent);

                if (!parentData)
                {
                    std::cerr << "Invalid parent for object "
                            << projectObjData.name << '\n';
                    continue;
                }

                projectObjData.parent = GET_FORMER_HASHED(parentData->id);
                std::cout << "Finished getting parent information.\n";
            }

            std::cout << "Making context.\n";
            SerializeContext ctx;
            ctx.resources = &world->resources;
            ctx.objectName = projectObjData.name;

            std::cout << "Gathering elements.\n";
            // Serialize every registered component.
            for (auto&& [type, storage] : objects.storage())
            {
                if (type == GetTypeID<ObjectData>())
                    continue;

                if (!storage.contains(object))
                    continue;

                std::cout << "Getting element of type: " << type << '\n';
                auto* entry = Reflection::TryGet(type);

                if (!entry)
                    continue;

                json elementJson;

                try
                {
                    std::cout << "Attempting to create data for element: " << entry->name << '\n';
                    elementJson = entry->to_json(
                        storage.value(object),
                        &ctx
                    );
                    std::cout << "Converted to json.\n";

                    auto elementData = entry->create_project_data(
                        elementJson,
                        entry->name
                    );
                    std::cout << "Made data.\n";

                    if (elementData)
                        projectObjData.elements.push_back(
                            std::move(elementData)
                        );
                }
                catch (const nlohmann::json::exception& e)
                {
                    std::cerr
                        << "\n[SCENE SERIALIZATION ERROR]\n"
                        << "Scene: " << data.name << '\n'
                        << "Object: " << projectObjData.name << '\n'
                        << "Element: " << entry->name << '\n'
                        << "Error: " << e.what() << '\n'
                        << "Serialized JSON:\n"
                        << elementJson.dump(2) << '\n';

                    throw;
                }
            }

            data.objects.push_back(std::move(projectObjData));
        }

        std::cout << "Finished turning scene into ProjectSceneData.\n";

        return data;
    }
}