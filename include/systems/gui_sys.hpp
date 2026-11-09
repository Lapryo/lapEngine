#pragma once

#include "system.hpp"
using namespace lapCore;

class GUISystem : public System
{
public:
    GUISystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {}

    void Update(
        float deltaTime, 
        registry &registry
    ) override;
    
    void ResetInUIList();

    void MarkUIDirty(
        Scene* scene,
        registry& registry,
        entity entity
    );

    void Connect(registry &registry);
    void OnUIUpdated(
        registry &registry, 
        Object entity
    );

    string GetName() const override 
    { return "gui"; }
};