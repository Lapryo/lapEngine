#ifndef SCENE_HPP
#define SCENE_HPP

#include "systems.hpp"
#include "resource_manager.hpp"

namespace lapCore
{
    struct Scene
    {
        std::string name;
        entt::registry entities;
        std::unordered_map<std::string, entt::entity> nameToEntity;
        std::unordered_map<SystemDrawOrder, std::vector<std::unique_ptr<System>>> systems;
        ResourceManager resources;

        void Update(float deltaTime, SystemDrawOrder order);
        void AddSystem(std::unique_ptr<System> system);

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