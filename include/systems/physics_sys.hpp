#ifndef PHYSICS_SYS_HPP
#define PHYSICS_SYS_HPP

#include "box2d/box2d.h"

#include "system.hpp"
using namespace lapCore;

class PhysicsSystem : public System
{
public:
    ~PhysicsSystem();
    PhysicsSystem(Scene *scene, unsigned int order) : System(order, scene, false) {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldID = b2CreateWorld(&worldDef);
    }
    void Update(float deltaTime, entt::registry &reg) override;

    b2BodyId Create2DBody(b2BodyDef bodyDef, b2ShapeDef shapeDef, b2Polygon polygon);

    //float noncontact_precision = 100.f; // higher = more precise
    //float contact_hitbox_precision = 0.1f; // higher = less precise hitboxes than intended (smaller)

    b2WorldId worldID;

    std::string GetName() const override { return "PhysicsSystem"; }
};

#endif