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
        entt::registry &registry
    ) override;
    
    void ResetInUIList();

    std::string GetName() const override 
    { return "GUISystem"; }
};