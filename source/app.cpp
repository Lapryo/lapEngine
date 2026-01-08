#include "app.hpp"

using namespace lapCore;

lapCore::App::App(Project &project) : project(project)
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

    for (auto &scene : project.scenes)
    {
        scene->Clear();
    }

    rl::UnloadRenderTexture(project.target);

    // shutdown window if it exists
    rl::CloseWindow();

    state = AppState::DEAD;
}