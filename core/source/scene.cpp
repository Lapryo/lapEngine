#include "scene.hpp"

using namespace lapCore;

void Scene::QueueAsset(const std::string &name, const std::string &type, const std::string &path)
{
    queuedAssets.push_back({name, type, path});
}

void Scene::QueueAsset(const AssetLoadRequest &asset)
{
    queuedAssets.push_back(asset);
}

void Scene::LoadQueuedAssets()
{
    for (auto &asset : queuedAssets)
    {
        if (asset.type == "texture")
            resources.AddTexture(asset.name, asset.path);
        else if (asset.type == "shader")
            resources.AddShader(asset.name, asset.path + "/vert.glsl", asset.path + "/frag.glsl");
    }

    queuedAssets.clear();
}

// figure this out later
void Scene::Update(float deltaTime, RenderTexture2D &target)
{
    resolutionScale = logicalResolution.x / LOGICAL_RESOLUTION_REFERENCE;

    for (auto &system : systems)
    {
        if (!system || !system->active)
            continue;

        bool drawing = false;
        if (system->drawing)
            drawing = true;

        if (drawing)
        {
            BeginDrawing();
            BeginTextureMode(target);
            ClearBackground(WHITE);
        }

        system->Update(deltaTime, objects);

        if (drawing)
        {
            EndTextureMode();
            ClearBackground(BLACK);

            // Now draw render texture to the screen, scaled and letterboxed
            int screenW = GetScreenWidth();
            int screenH = GetScreenHeight();
            float screenAspect = (float)screenW / screenH;
            float targetAspect = (float)logicalResolution.x / logicalResolution.y;

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

            logicalWindowPos = {(float)offsetX, (float)offsetY};

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
    }
}

Object Scene::AddObject(const std::string &name, const std::string &parent)
{
    auto object = objects.create();

    ObjectEntry entry;
    entry.object = object;
    entry.parent = objectMap[parent].object;
    objectMap[name] = entry;

    objectMap[parent].children.push_back(object);

    return object;
}

void Scene::RemoveObject(Object &object)
{
    objects.destroy(object);
}

void Scene::Clear()
{
    objects.clear();
    systems.clear();

    for (auto &texture : resources.textures)
    {
        resources.RemoveTexture(texture.first);
    }

    for (auto &shader : resources.shaders)
    {
        resources.RemoveShader(shader.first);
    }
}

Object Scene::FindObject(const std::string &name)
{
    return objectMap[name].object;
}

std::string lapCore::Scene::GetObjectName(Object &object)
{
    for (auto entry : objectMap)
    {
        if (entry.second.object == object)
            return entry.first;
    }

    return "";
}

std::vector<Object> lapCore::Scene::GetChildren(Object &object)
{
    return objectMap[GetObjectName(object)].children;
}

Object lapCore::Scene::FindChild(Object &object, const std::string &name)
{
    std::string objName = GetObjectName(object);
    for (auto &child : objectMap[objName].children)
    {
        if (GetObjectName(child) == name)
            return child;
    }
}
