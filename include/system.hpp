#pragma once

#include <entt/entt.hpp>

namespace lapCore
{
    using namespace std;
    using namespace entt;

    struct Scene;
    
    using Object = entity;

    class System
    {
    public:
        System(unsigned int order, Scene *scene) : order(order), scene(scene) {}
        virtual ~System() = default;
        virtual void Update(float deltaTime, registry &reg) = 0;
        virtual string GetName() const = 0;
        virtual void Disconnect(registry&) {} // Remove EnTT callbacks before teardown.

        unsigned int order;
        bool active = true;
        Scene *scene;
    };
}