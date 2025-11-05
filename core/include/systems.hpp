#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "entt/entt.hpp"
#include "components.hpp"

namespace lapCore
{
    enum class SystemDrawOrder
    {
        PREDRAW,
        DRAW,
        POSTDRAW
    };

    class System
    {
    public:
        System(SystemDrawOrder order) : drawOrder(order) {}
        virtual ~System() = default;
        virtual void Update(float deltaTime, entt::registry &reg) = 0;

        SystemDrawOrder drawOrder;
    };

    class PhysicsSystem : public System
    {
    public:
        PhysicsSystem() : System(SystemDrawOrder::PREDRAW) {}
        void Update(float deltaTime, entt::registry &reg) override;
    };

    class RenderSystem : public System
    {
    public:
        RenderSystem() : System(SystemDrawOrder::DRAW) {}
        void Update(float deltaTime, entt::registry &reg) override;
    };
}

#endif