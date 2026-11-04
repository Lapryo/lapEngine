#pragma once

#include "system.hpp"
using namespace lapCore;

class ScriptSystem : public System
{
public:
    ScriptSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {}

    void Update(
        float deltaTime, 
        registry &registry
    ) override;

    void OnDestroy(registry &registry);

    string GetName() const override 
    { return "ScriptSystem"; }
};