#include "scene.hpp"
#include "world.hpp"

using namespace lapCore;

Object lapCore::ObjectContainer::AddObject(entt::hashed_string name, entt::hashed_string parent, int childIndex, bool fromPrefab)
{
    ObjectEntry entry;
    entry.info.id = name.value();
    entry.info.object = this->objects.create();
    entry.parent.id = parent.value();
    entry.childIndex = childIndex;
    entry.fromPrefab = fromPrefab;

    objectMap[name.value()] = entry;
    return entry.info.object;
}

void lapCore::ObjectContainer::RemoveObject(entt::id_type id)
{
    auto object = *FindObject(id);

    objects.destroy(object);
    objectMap.erase(id);
}

void lapCore::ObjectContainer::RemoveObject(Object object)
{
    for (const auto &obj : objectMap)
    {
        if (obj.second.info.object == object)
        {
            objects.destroy(obj.second.info.object);
            objectMap.erase(obj.second.info.id);
        }
    }
}

Object *lapCore::ObjectContainer::FindObject(entt::id_type id)
{
    auto it = objectMap.find(id);
    if (it != objectMap.end())
        return &it->second.info.object;
    return nullptr;
}

ObjectEntry *lapCore::ObjectContainer::FindEntry(Object object)
{
    for (auto& objEntry : objectMap)
    {
        if (objEntry.second.info.object == object)
            return &objEntry.second;
    }
    return nullptr;
}

void lapCore::ObjectContainer::AddObjectsFromProjectData(std::vector<ProjectObjectData> objects)
{
    for (const auto& object_data : objects)
    {
        auto prefabObject = AddObject(HASH(object_data.name.c_str()), HASH(object_data.parent.c_str()), object_data.child_index, true);
        for (const auto& element : object_data.elements)
        {
            if (!element.get())
                continue;

            AddElement(HASH(object_data.name.c_str()), element->GetTypeID(), element->GetDataPtr());
        }
    }
}

void lapCore::ObjectContainer::AddElement(entt::hashed_string objectName, entt::id_type elementType, void *elementData)
{
    auto it = objectMap.find(objectName.value());
    if (it == objectMap.end())
        return;

    AddElement(it->second.info.object, elementType, elementData);
}

void lapCore::ObjectContainer::AddElement(Object object, entt::id_type elementType, void *elementData)
{
    auto entry = Reflection::TryGet(elementType);
    entry->emplace(objects, object, elementData);
}

void lapCore::ObjectContainer::RemoveElement(entt::id_type id, entt::id_type elementType)
{
    auto it = objectMap.find(id);
    if (it == objectMap.end())
        return;

    RemoveElement(it->second.info.object, elementType);
}

void lapCore::ObjectContainer::RemoveElement(Object object, entt::id_type elementType)
{
    auto entry = Reflection::TryGet(elementType);
    entry->erase(objects, object);
}

void *lapCore::ObjectContainer::FindElement(entt::id_type id, entt::id_type elementType)
{
    auto it = objectMap.find(id);
    if (it == objectMap.end())
        return nullptr;

    return FindElement(it->second.info.object, elementType);
}

void *lapCore::ObjectContainer::FindElement(Object object, entt::id_type elementType)
{
    auto* storage = objects.storage(elementType);
    if (!storage)
        return nullptr;

    if (!storage->contains(object))
        return nullptr;

    return storage->value(object);
}

void lapCore::ObjectContainer::ClearObjects()
{
    objects.clear();
    objectMap.clear();
}

void lapCore::Scene::Update(float deltaTime, RenderTexture2D &target)
{
    /// Draw to render texture first
    BeginTextureMode(target);
    ClearBackground(WHITE);

    for (auto &[order, system] : systems)
    {
        if (!system || !system->active)
            continue;

        bool drawing = false;
        if (system->drawing)
            drawing = true;

        system->Update(deltaTime, objects);
    }

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

    EndDrawing();
}

void lapCore::Scene::Clear()
{
    ClearObjects();
    systems.clear();
    name = "";
}