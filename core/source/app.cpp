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
        const float delta = GetFrameTime();
        Update(delta);
        Draw();
    }
}

void App::Shutdown()
{
    for (auto &scene : project.scenes)
    {
        scene->Clear();
    }

    // shutdown window if it exists
    CloseWindow();
}