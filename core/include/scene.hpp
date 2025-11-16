#ifndef SCENE_HPP
#define SCENE_HPP

#include "systems.hpp"
#include "resource_manager.hpp"

#include <iostream>

namespace lapCore
{
    const unsigned int LOGICAL_RESOLUTION_REFERENCE = 6400;

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
        std::unordered_map<entt::entity, entt::entity> entityToParent;
        std::unordered_map<entt::entity, std::vector<entt::entity>> entityToChildren;
        std::vector<std::unique_ptr<System>> systems;
        std::vector<AssetLoadRequest> queuedAssets;
        ResourceManager resources;

        Vector2 logicalWindowPos;
        Vector2 logicalResolution;
        double resolutionScale = 1;

        void QueueAsset(const std::string &name, const std::string &type, const std::string &path);
        void QueueAsset(const AssetLoadRequest &asset);

        void LoadQueuedAssets();
        void ReloadTextures();

        void Update(float deltaTime, RenderTexture2D &target);

        template <typename T, typename... Args>
        void AddSystem(unsigned int order, Args&&... args)
        {
            std::cout << "got to here.\n";
            auto sys = std::make_unique<T>(this, order, std::forward<Args>(args)...);
            std::cout << "got to here.\n";

            if constexpr (requires(T& t, entt::registry& r) { t.Connect(r); })
                sys->Connect(entities);
            
            std::cout << "got to here.\n";

            if (order > systems.size())
                systems.resize(order + 1);

            systems[order] = std::move(sys);
            std::cout << "got to here.\n";
        }

        entt::entity AddEntity(const std::string &name, const std::string &parent);
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