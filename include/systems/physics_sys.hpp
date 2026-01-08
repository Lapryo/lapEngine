#ifndef PHYSICS_SYS_HPP
#define PHYSICS_SYS_HPP

#include "system.hpp"
using namespace lapCore;

class PhysicsSystem : public System
{
public:
    PhysicsSystem(Scene *scene, unsigned int order) : System(order, scene, false) {}
    void Update(float deltaTime, entt::registry &reg) override;
};

#endif