#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "entt/entt.hpp"
#include "components.hpp"

namespace lapCore
{
    using Object = entt::entity;

    class System
    {
    public:
        System(unsigned int order, Scene *scene, bool isDrawing) : order(order), scene(scene), drawing(isDrawing) {}
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
        PhysicsSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
        void Update(float deltaTime, entt::registry &reg) override;
    };

    class RenderSystem : public System
    {
    public:
        struct RenderEntry
        {
            Object entity;
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

        RenderSystem(Scene *scene, unsigned int order) : System(order, scene, true) {}
        void Update(float deltaTime, entt::registry &reg) override;

        void Connect(entt::registry &registry);
        void OnRenderableUpdated(entt::registry &registry, Object entity);

        void RebuildRenderList(entt::registry &registry);

        bool needsResort = true;
    };

    class ScriptSystem : public System
    {
    public:
        ScriptSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
        void Update(float deltaTime, entt::registry &registry) override;
        void OnDestroy(entt::registry &registry);
    };

    class GUISystem : public System
    {
    public:
        GUISystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
        void Update(float deltaTime, entt::registry &registry) override;
    };
}

#endif