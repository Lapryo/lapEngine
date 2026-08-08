#pragma once

#include "system.hpp"
using namespace lapCore;

class RenderSystem : public System
{
public:
    // TODO: Add support for models and custom 2d shapes
    enum class RenderType
    {
        Sprite,
        Text,
        Rect,
        Ellipse,
        Image
    };

    struct RenderEntry
    {
        Object entity;
        int zlayer, ySort;
        bool isScreenSpace;
        RenderType type;
    };

    RenderSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {}

    void Update(
        float deltaTime, 
        entt::registry &reg
    ) override;

    void Connect(entt::registry &registry);
    void OnRenderableUpdated(
        entt::registry &registry, 
        Object entity
    );

    void RebuildRenderList(entt::registry &registry);

    std::vector<RenderEntry> renderList;
    bool needsResort = true;

    std::string GetName() const override 
    { return "RenderSystem"; }
};