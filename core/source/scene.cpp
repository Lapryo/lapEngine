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
        sprite.texture = &resources.textures[sprite.texName];
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

void Scene::Update(float deltaTime, SystemDrawOrder order)
{
    for (auto &system : systems[order])
    {
        if (system->active) {
            system->Update(deltaTime, entities);
        }
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