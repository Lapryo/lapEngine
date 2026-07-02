#pragma once

#include <entt/entt.hpp>
#include "elements.hpp"

namespace lapCore
{
    struct Scene;
    
    using Object = entt::entity;

    class System
    {
    public:
        System(unsigned int order, Scene *scene) : order(order), scene(scene) {}
        virtual ~System() = default;
        virtual void Update(float deltaTime, entt::registry &reg) = 0;
        virtual std::string GetName() const = 0;

        unsigned int order;
        bool active = true;
        Scene *scene;
    };
}