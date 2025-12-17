#include "app.hpp"

using namespace lapCore;

lapCore::App::App(Project &project)
{
    this->project = std::move(project);
}

void App::Run()
{
    while (state == AppState::RUNNING)
    {
        if (rl::WindowShouldClose())
        {
            state = AppState::DEAD;
            return;
        }

        const float delta = rl::GetFrameTime();
        Update(delta);
    }
}

void App::Shutdown()
{
    for (auto &scene : project.scenes)
    {
        scene->Clear();
    }

    rl::UnloadRenderTexture(project.target);

    // shutdown window if it exists
    rl::CloseWindow();
}