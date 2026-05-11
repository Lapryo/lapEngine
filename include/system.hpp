#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "entt.hpp"
#include "elements.hpp"

namespace lapCore
{
    using Object = entt::entity;

    class System
    {
    public:
        System(unsigned int order, Scene *scene, bool isDrawing) : order(order), scene(scene), drawing(isDrawing) {}
        virtual ~System() = default;
        virtual void Update(float deltaTime, entt::registry &reg) = 0;
        virtual std::string GetName() const = 0;

        unsigned int order;
        bool active = true;
        bool drawing = false;
        Scene *scene;
    };
}

#endif