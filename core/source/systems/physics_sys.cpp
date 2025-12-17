#include "systems.hpp"

using namespace lapCore;

void PhysicsSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Origin2D, Physics2D>();
    for (auto [entity, origin, physics] : view.each())
    {
        origin.position.x += physics.velocity.x * deltaTime;
        origin.position.y += physics.velocity.y * deltaTime;
    }
}