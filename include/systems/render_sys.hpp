#ifndef RENDER_SYS_HPP
#define RENDER_SYS_HPP

#include "system.hpp"
using namespace lapCore;

class RenderSystem : public System
{
public:
    enum class RenderType
    {
        Sprite,
        Text,
        Rect,
        Image
    };

    struct RenderEntry
    {
        Object entity;
        unsigned int zlayer;
        bool isScreenSpace;
        RenderType type;
    };

    std::vector<RenderEntry> renderList;

    RenderSystem(Scene *scene, unsigned int order) : System(order, scene, true) {}
    void Update(float deltaTime, entt::registry &reg) override;

    void Connect(entt::registry &registry);
    void OnRenderableUpdated(entt::registry &registry, Object entity);

    void RebuildRenderList(entt::registry &registry);

    bool needsResort = true;

    std::string GetName() const override { return "RenderSystem"; }
};

#endif