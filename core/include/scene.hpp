#ifndef SCENE_HPP
#define SCENE_HPP

#include "systems.hpp"

namespace lapCore
{
    struct Scene
    {
        std::string name;
        entt::registry entities;
        std::vector<System*> systems;

        void Update(float deltaTime);
        void AddSystem(System* system);

        entt::entity AddEntity();
        void DestroyEntity(entt::entity &entity);
        void FindEntity();

        template <typename Comp>
        Comp &AddComponent(entt::entity &entity, Comp &component)
        {
            return entities.emplace<Comp>(entity, std::forward<Comp>(component));
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