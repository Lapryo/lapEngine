#include "hub.hpp"
#include <iostream>

using namespace lapHub;
using namespace lapCore;

HubApp::HubApp(Project &project) : App(project)
{}

void HubApp::Init()
{
    InitWindow(800, 450, "lapHub");
    SetTargetFPS(60);

    state = AppState::RUNNING;
}

void HubApp::Update(float deltaTime)
{
    if (WindowShouldClose())
    {
        state = AppState::DEAD;
        return;
    }

    project.scenes[0].Update(deltaTime);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}

void HubApp::Draw()
{

}

int main()
{
    Project hubProject = UnpackProject("assets/proj.lapengine.json");
    HubApp hub(hubProject);

    hub.Init();
    hub.Run();
    hub.Shutdown();

    return 0;
}