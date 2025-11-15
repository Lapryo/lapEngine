#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "entt/entt.hpp"
#include "components.hpp"

namespace lapCore
{
    class System
    {
    public:
        System(unsigned int order, Scene *scene) : order(order), scene(scene) {}
        virtual ~System() = default;
        virtual void Update(float deltaTime, entt::registry &reg) = 0;

        unsigned int order;
        bool active = true;
        bool drawing = false;
        Scene *scene;
    };

    class PhysicsSystem : public System
    {
    public:
        PhysicsSystem(Scene *scene, unsigned int order) : System(order, scene) {}
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
            enum class Type
            {
                Sprite,
                Text,
                Rect,
                RoundedRect,
                Circle,
                Image
            } type;
        };

        std::vector<RenderEntry> renderList;

        RenderSystem(Scene *scene, unsigned int order) : System(order, scene) {drawing = true;}
        void Update(float deltaTime, entt::registry &reg) override;

        void Connect(entt::registry &registry);
        void OnRenderableUpdated(entt::registry &registry, entt::entity entity);

        void RebuildRenderList(entt::registry &registry);

        bool needsResort = true;
    };

    class ScriptSystem : public System
    {
    public:
        ScriptSystem(Scene *scene, unsigned int order) : System(order, scene) {}
        void Update(float deltaTime, entt::registry &registry) override;
        void OnDestroy(entt::registry &registry);
    };

    class GUISystem : public System
    {
    public:
        GUISystem(Scene *scene, unsigned int order) : System(order, scene) {}
        void Update(float deltaTime, entt::registry &registry) override;
    };
}

#endif