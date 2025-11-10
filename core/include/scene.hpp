#ifndef SCENE_HPP
#define SCENE_HPP

#include "systems.hpp"
#include "resource_manager.hpp"

namespace lapCore
{
    struct AssetLoadRequest
    {
        std::string name;
        std::string type;
        std::string path;
    };

    struct Scene
    {
        std::string name;
        entt::registry entities;
        std::unordered_map<std::string, entt::entity> nameToEntity;
        std::unordered_map<SystemDrawOrder, std::vector<std::unique_ptr<System>>> systems;
        std::vector<AssetLoadRequest> queuedAssets;
        ResourceManager resources;

        Vector2 logicalWindowPos;
        Vector2 logicalResolution;

        void QueueAsset(const std::string &name, const std::string &type, const std::string &path);
        void QueueAsset(const AssetLoadRequest &asset);

        void LoadQueuedAssets();
        void ReloadTextures();

        void Update(float deltaTime, SystemDrawOrder order, RenderTexture2D &target);

        template <typename T, typename... Args>
        void AddSystem(Args&&... args)
        {
            auto sys = std::make_unique<T>(this, std::forward<Args>(args)...);

            if constexpr (requires(T& t, entt::registry& r) { t.Connect(r); })
                sys->Connect(entities);

            systems[sys->drawOrder].push_back(std::move(sys));
        }

        entt::entity AddEntity(const std::string &name);
        void DestroyEntity(entt::entity &entity);
        entt::entity FindEntity(const std::string &name);

        template <typename Comp, typename... CompArgs>
        Comp &AddComponent(entt::entity &entity, CompArgs&&... args)
        {
            return entities.emplace<Comp>(entity, std::forward<CompArgs>(args)...);
        }

        template <typename Comp>
        void DestroyComponent(entt::entity &entity)
        {
            entities.remove<Comp>(entity);
        }

        void Clear();
        void ClearComponents();
    };
}

#endif