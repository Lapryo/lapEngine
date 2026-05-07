#include "core.hpp"

using namespace lapCore;

PhysicsSystem::~PhysicsSystem()
{
    b2DestroyWorld(worldID);
}

b2BodyId PhysicsSystem::Create2DBody(b2BodyDef bodyDef, b2ShapeDef shapeDef, b2Polygon polygon)
{
    b2BodyId bodyId = b2CreateBody(worldID, &bodyDef);
    b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
    return bodyId;
}

void PhysicsSystem::Update(float deltaTime, entt::registry &registry)
{
    b2World_Step(worldID, deltaTime, 4);


    auto view = registry.view<Origin2D, Physics2D>();
    for (auto [entity, origin, physics] : view.each())
    {
        if (b2Body_IsValid(physics.bodyID))
        {
            auto bodyPos = b2Body_GetPosition(physics.bodyID);
            origin.position = {bodyPos.x, bodyPos.y};
        }
        /*
        // implement collisions
        Vector2 oldPos = origin.position;
        Vector2 newPos = {origin.position.x + physics.velocity.x * deltaTime - physics.gravity.x * deltaTime,
                              origin.position.y + physics.velocity.y * deltaTime - physics.gravity.y * deltaTime};

        if (physics.collidable)
        {
            // detect if the object will be collided with something at the new position point
            Rectangle thisRect;
            thisRect.x = newPos.x + (contact_hitbox_precision / 2.f);
            thisRect.y = newPos.y - (contact_hitbox_precision / 2.f);
            thisRect.width = physics.hitbox.x - contact_hitbox_precision;
            thisRect.height = physics.hitbox.y - contact_hitbox_precision;
            
            // loop through all objects with an origin and physics besides this one
            for (auto [e2, o2, p2] : view.each())
            {
                if (e2 != entity && p2.collidable)
                {
                    Rectangle otherRect;
                    otherRect.x = o2.position.x + (contact_hitbox_precision / 2.f);
                    otherRect.y = o2.position.y - (contact_hitbox_precision / 2.f);
                    otherRect.width = p2.hitbox.x - contact_hitbox_precision;
                    otherRect.height = p2.hitbox.y - contact_hitbox_precision;

                    if (CheckCollisionRecs(thisRect, otherRect))
                    {
                        // get the direction between the old position and the new position
                        Vector2 difference = {newPos.x - oldPos.x, newPos.y - oldPos.y};
                        auto dir = rm::Vector2Normalize(difference);
                        if (dir.x == 0 && dir.y == 0) dir = {1.f, 1.f};

                        // repeat decreasing the distance until CheckCollisionRecs is false
                        while (CheckCollisionRecs(thisRect, otherRect))
                        {
                            thisRect.x -= dir.x / noncontact_precision;
                            thisRect.y -= dir.y / noncontact_precision;
                        }

                        if (physics.bounce)
                        {
                            auto dot = rm::Vector2DotProduct()
                        }

                        // check if bounce is true
                        // if so, calculate dot product and change velocity to that direction with the same magnitude

                        newPos.x = thisRect.x - (contact_hitbox_precision / 2.f);
                        newPos.y = thisRect.y + (contact_hitbox_precision / 2.f);
                    }
                }
            }
        }

        origin.position.x = newPos.x;
        origin.position.y = newPos.y;*/
    }
}