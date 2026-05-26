#include "core.hpp"
#include "systems/physics_sys.hpp"

using namespace lapCore;

PhysicsSystem::~PhysicsSystem()
{
    b2DestroyWorld(worldID);
}

void PhysicsSystem::RegisterBodies()
{
    auto view = scene->objects.view<Physics2D>();
    for (auto [e, physics] : view.each())
    {
        if (!b2Body_IsValid(bodyMap[e]))
            bodyMap[e] = Create2DBody(physics.bodyDef, physics.shapeDef, physics.polygon);
    }
}

b2BodyId PhysicsSystem::Create2DBody(b2BodyDef bodyDef, b2ShapeDef shapeDef, b2Polygon polygon)
{
    b2BodyId bodyId = b2CreateBody(worldID, &bodyDef);
    b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    return bodyId;
}

const float timeStep = 1.f / 60.f;

void PhysicsSystem::Update(float deltaTime, entt::registry &registry)
{
    static float accumulator = 0.f;
    accumulator += deltaTime;
    while (accumulator >= timeStep)
    {
        b2World_Step(worldID, timeStep, 12);
        accumulator -= timeStep;
    }

    auto view = registry.view<Transform2D, Physics2D>();
    for (auto [entity, origin, physics] : view.each())
    {
        if (b2Body_IsValid(bodyMap[entity]))
        {
            auto bodyPos = b2Body_GetPosition(bodyMap[entity]);
            origin.position = {bodyPos.x, bodyPos.y};
            b2Rot rotation = b2Body_GetRotation(bodyMap[entity]);
            float radians = atan2f(rotation.s, rotation.c);
            float degrees = radians * RAD2DEG;
            if (degrees < 0) degrees += 360.0f;
            origin.rotation = degrees;
        }
    }
}