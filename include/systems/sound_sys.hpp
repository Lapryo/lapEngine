#pragma once

#include "system.hpp"
using namespace lapCore;

class SoundSystem : public System
{
public:
    SoundSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {}

    void Update(
        float deltaTime, 
        entt::registry &reg
    ) override;

    void StopAll();

    std::string GetName() const override 
    { return "SoundSystem"; }
};