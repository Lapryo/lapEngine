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

void Scene::ReloadTextures()
{
    auto view = entities.view<Sprite>();
    for (auto entity : view)
    {
        auto &sprite = view.get<Sprite>(entity);
        sprite.texture = &resources.textures[sprite.textureName];
    }
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
    ReloadTextures();
}

void Scene::Update(float deltaTime, SystemDrawOrder order, RenderTexture2D &target)
{
    resolutionScale = logicalResolution.x / LOGICAL_RESOLUTION_REFERENCE;

    if (order == SystemDrawOrder::DRAW)
    {
        BeginTextureMode(target);
        ClearBackground(WHITE);
    }

    for (auto &system : systems[order])
    {
        if (system->active) {
            system->Update(deltaTime, entities);
        }
    }

    if (order == SystemDrawOrder::DRAW)
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

        if (screenAspect > targetAspect) {
            // window is wider than logical
            drawHeight = screenH;
            drawWidth = (int)(screenH * targetAspect);
            offsetX = (screenW - drawWidth) / 2;
            offsetY = 0;
        } else {
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
            { 0.0f, 0.0f, (float)target.texture.width, -(float)target.texture.height }, // source rect (flip y)
            { (float)offsetX, (float)offsetY, (float)drawWidth, (float)drawHeight },     // dest rect
            { 0.0f, 0.0f }, // origin
            0.0f,           // rotation
            WHITE
        );

        EndDrawing();
    }
}

entt::entity Scene::AddEntity(const std::string &name)
{
    const auto entity = entities.create();
    nameToEntity[name] = entity;
    return entity;
}

void Scene::DestroyEntity(entt::entity& entity)
{
    entities.destroy(entity);
}

void Scene::Clear()
{
    entities.clear();
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

entt::entity Scene::FindEntity(const std::string &name)
{
    return nameToEntity[name];
}