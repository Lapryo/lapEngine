#include "core.hpp"

#include <iostream>

using namespace lapCore;

int main()
{
    Scene main_scene = CreateScene();
    PhysicsSystem physics = PhysicsSystem();
    main_scene.AddSystem(&physics);

    auto entity = main_scene.AddEntity();

    Transform2D transform;
    transform.position = {0.0f, 0.0f};
    transform.velocity = {1.0f, -1.0f};
    transform.scale = {0.0f, 0.0f};
    transform.rotation = 0.0f;

    main_scene.AddComponent<Transform2D>(entity, transform);

    std::cout << "X: " << transform.position.x << " | Y: " << transform.position.y << "\n";

    for (int i = 0; i < 10; ++i)
    {
        main_scene.Update(0);
    }

    main_scene.Clear();

    return 0;
}