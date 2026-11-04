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
        registry &reg
    ) override;

    void StopAll();

    string GetName() const override 
    { return "SoundSystem"; }
};