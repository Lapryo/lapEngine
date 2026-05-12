#include "scene.hpp"
#include "world.hpp"

using namespace lapCore;

void Scene::Update(float deltaTime, RenderTexture2D &target)
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

Object Scene::AddObject(const std::string &name, const std::string &parent, int childIndex)
{
    auto object = objects.create();

    // Create object info
    ObjectInfo objInfo;
    objInfo.name = name;
    objInfo.object = object;

    // Create parent info
    ObjectInfo parentInfo;
    parentInfo.name = parent;
    if (parent == "")
        parentInfo.object = entt::null;
    else
        parentInfo.object = objectMap[parent].info.object;

    // Create the object entry for the map
    ObjectEntry entry;
    entry.info = objInfo;
    entry.parent = parentInfo;
    entry.childIndex = childIndex;

    objectMap[name] = entry;

    if (childIndex == -1)
    {
        if (parent != "")
        {
            objectMap[parent].children.push_back(objInfo);
        }
    }
    else
    {
        if (parent != "")
        {
            auto &children = objectMap[parent].children;
            if (children.size() < (size_t)(childIndex + 1))
            {
                size_t old = children.size();
                children.resize(childIndex + 1);
                for (size_t k = old; k < children.size(); ++k)
                {
                    children[k].name = "";
                    children[k].object = entt::null;
                }
            }

            children[childIndex] = objInfo;
        }
    }

    return object;
}

void Scene::RemoveObject(Object object)
{
    objects.destroy(object);
}

void Scene::Clear()
{
    objects.clear();
    systems.clear();
    prefabs.clear();

    objectMap.clear();
    prefabMap.clear();
}

ObjectEntry Scene::FindObject(const std::string &name)
{
    auto it = objectMap.find(name);
    if (it != objectMap.end())
        return it->second;

    dbgln("Warning: Object '" + name + "' not found in scene '" + this->name + "'", LogType::WARNING);
    return ObjectEntry();
}

Object lapCore::Scene::AddPrefab(const std::string &name, const std::string &parent, int childIndex)
{
    auto object = prefabs.create();

    // Create object info
    ObjectInfo objInfo;
    objInfo.name = name;
    objInfo.object = object;

    // Create parent info
    ObjectInfo parentInfo;
    parentInfo.name = parent;
    parentInfo.object = prefabMap[parent].info.object;

    // Create the object entry for the map
    ObjectEntry entry;
    entry.info = objInfo;
    entry.parent = parentInfo;
    entry.childIndex = childIndex;

    prefabMap[name] = entry;

    if (childIndex == -1)
        prefabMap[parent].children.push_back(objInfo);
    else
    {
        auto &children = prefabMap[parent].children;
        if (children.size() < (size_t)(childIndex + 1))
        {
            size_t old = children.size();
            children.resize(childIndex + 1);
            for (size_t k = old; k < children.size(); ++k)
            {
                children[k].name = "";
                children[k].object = entt::null;
            }
        }

        children[childIndex] = objInfo;
    }

    return object;
}

ObjectEntry lapCore::Scene::FindPrefab(const std::string &name)
{
    if (prefabMap.find(name) != prefabMap.end())
        return prefabMap[name];

    return ObjectEntry();
}

Object lapCore::Scene::AddObjectFromPrefab(const std::string &prefabName, const std::string &newName)
{
    auto object = objects.create();

    ObjectEntry entry;
    entry = FindPrefab(prefabName);

    entry.info.object = object;
    entry.info.name = newName;
    objectMap[newName] = entry;

    return object;
}

std::string lapCore::Scene::GetObjectName(Object object)
{
    for (auto entry : objectMap)
    {
        if (entry.second.info.object == object)
            return entry.second.info.name;
    }

    return "";
}

std::vector<ObjectInfo> lapCore::Scene::GetChildren(Object object)
{
    if (GetObjectName(object) == "")
        return {};

    return objectMap[GetObjectName(object)].children;
}

Object lapCore::Scene::FindChild(Object object, const std::string &name)
{
    std::string objName = GetObjectName(object);
    if (objName == "")
        return Object{entt::null};
    for (auto &child : objectMap[objName].children)
    {
        if (child.name == name)
            return child.object;
    }
    return Object{entt::null};
}

void lapCore::Scene::SetParent(const std::string &name, Object parent)
{
    FindObject(name).parent = FindObject(GetObjectName(parent)).info;
}

ObjectInfo lapCore::Scene::GetParent(const std::string &name)
{
    return FindObject(name).parent;
}