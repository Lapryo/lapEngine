#ifndef SCRIPT_SYS_HPP
#define SCRIPT_SYS_HPP

#include "system.hpp"
using namespace lapCore;

class ScriptSystem : public System
{
public:
    ScriptSystem(Scene *scene, unsigned int order) : System(order, scene, true) {}
    void Update(float deltaTime, entt::registry &registry) override;
    void OnDestroy(entt::registry &registry);

    std::string GetName() const override { return "ScriptSystem"; }
};

#endif