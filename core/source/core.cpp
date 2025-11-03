#include "core.hpp"

#include <utility>
#include <iostream>

using namespace lapCore;

void PhysicsSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Transform2D>();
    for (auto entity : view)
    {
        auto &transform = view.get<Transform2D>(entity);

        transform.position.x += transform.velocity.x;
        transform.position.y += transform.velocity.y;

        std::cout << "X: " << transform.position.x << " | Y: " << transform.position.y << "\n";
    }
}

void Scene::Update(float deltaTime)
{
    for (System* system : systems)
    {
        if (system) {
            system->Update(deltaTime, entities);
        }
    }
}

void Scene::AddSystem(System* system)
{
    if (system) {
        systems.push_back(system);
    }
}

entt::entity Scene::AddEntity()
{
    const auto entity = entities.create();
    return entity;
}

void Scene::DestroyEntity(entt::entity& entity)
{
    entities.destroy(entity);
}

void Scene::Clear()
{
    entities.clear();
    for (auto* system : systems) {
        delete system;
    }
    systems.clear();
}

Scene lapCore::CreateScene()
{
    Scene scene;
    return scene;
}

Scene lapCore::CreateScene(entt::registry &entities)
{
    Scene scene;
    scene.entities = std::move(entities);
    return scene;
}