#include "app.hpp"
#include "scene.hpp"

#include <iostream>

using namespace lapCore;

lapCore::App::App(Project &project) : world(project)
{}

void App::Run()
{
    if (state != AppState::DEAD) // If the app is not dead, and/or is running, then do nothing
        return;

    if (!Init())
    {
        state = AppState::ERROR;
        std::cerr << "Error: App initialization failed!\n";
        Shutdown();
        return;
    }

    state = AppState::RUNNING;

    while (state == AppState::RUNNING)
    {
        if (rl::WindowShouldClose())
        {
            Shutdown();
            return;
        }

        Update(rl::GetFrameTime());
    }
}

void App::Shutdown()
{
    if (state == AppState::DEAD) // If the app is already dead, do nothing
        return;

    for (auto &scene : world.scenes)
        scene->Clear();

    rl::UnloadRenderTexture(world.target);

    // shutdown window if it exists
    rl::CloseWindow();

    state = AppState::DEAD;
}