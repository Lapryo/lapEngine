#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "entt/entt.hpp"
#include "components.hpp"

namespace lapCore
{
    class System
    {
    public:
        virtual ~System() = default;
        virtual void Update(float deltaTime, entt::registry &reg) = 0;
    };

    class PhysicsSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry &reg) override;
    };

    class RenderSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry &reg) override;
    };
}

#endif