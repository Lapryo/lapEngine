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
        System(SystemDrawOrder order, Scene* scene) : drawOrder(order), scene(scene) {}
        virtual ~System() = default;
        virtual void Update(float deltaTime, entt::registry &reg) = 0;

        SystemDrawOrder drawOrder;
        bool active = true;
        Scene* scene;
    };

    class PhysicsSystem : public System
    {
    public:
        PhysicsSystem(Scene* scene) : System(SystemDrawOrder::PREDRAW, scene) {}
        void Update(float deltaTime, entt::registry &reg) override;
    };

    class RenderSystem : public System
    {
    public:
        struct RenderEntry
        {
            entt::entity entity;
            unsigned int zlayer;
            bool isScreenSpace;
            enum class Type { Sprite, Text, Rect } type;
        };

        std::vector<RenderEntry> renderList;

        RenderSystem(Scene* scene) : System(SystemDrawOrder::DRAW, scene) {}
        void Update(float deltaTime, entt::registry &reg) override;
        
        void Connect(entt::registry &registry);
        void OnRenderableUpdated(entt::registry &registry, entt::entity entity);

        void RebuildRenderList(entt::registry &registry);

        bool needsResort = true;
    };

    class ScriptSystem : public System
    {
    public:
        ScriptSystem(Scene* scene) : System(SystemDrawOrder::PREDRAW, scene) {}
        void Update(float deltaTime, entt::registry& registry) override;
        void OnDestroy(entt::registry& registry);
    };

    class GUISystem : public System
    {
    public:
        GUISystem(Scene* scene) : System(SystemDrawOrder::PREDRAW, scene) {}
        void Update(float deltaTime, entt::registry& registry) override;
    };
}

#endif