#pragma once

#include "box2d/box2d.h"

#include "system.hpp"
using namespace lapCore;

class PhysicsSystem : public System
{
public:
    ~PhysicsSystem();
    PhysicsSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) 
    {
        // Create the physics world
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldID = b2CreateWorld(&worldDef);

        RegisterBodies();
    }

    void Update(
        float deltaTime, 
        entt::registry &reg
    ) override;

    void RegisterBodies();
    b2BodyId Create2DBody(
        b2BodyDef bodyDef, 
        b2ShapeDef shapeDef, 
        b2Polygon polygon
    );

    b2WorldId worldID;
    std::unordered_map<Object, b2BodyId> bodyMap;

    std::string GetName() const override 
    { return "PhysicsSystem"; }
};