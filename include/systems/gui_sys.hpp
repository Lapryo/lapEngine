#pragma once

#include "system.hpp"
using namespace lapCore;

class GUISystem : public System
{
public:
    GUISystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
    void Update(float deltaTime, entt::registry &registry) override;
    std::string GetName() const override { return "GUISystem"; }

    void ResetInUIList();
};