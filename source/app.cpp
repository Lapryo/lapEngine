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

    dbgln("Completed initialization.", LogType::INFO);
    state = AppState::RUNNING;

    while (state == AppState::RUNNING)
    {
        if (WindowShouldClose())
        {
            Shutdown();
            return;
        }

        Update(GetFrameTime());
    }
}

void App::Shutdown()
{
    if (state == AppState::DEAD) // If the app is already dead, do nothing
        return;

    world.main_scene.Clear();

    UnloadRenderTexture(world.window.target);

    // shutdown window if it exists
    CloseWindow();

    state = AppState::DEAD;
}