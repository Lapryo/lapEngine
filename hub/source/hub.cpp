#include "hub.hpp"
#include "projdata.hpp"
#include <iostream>

using namespace lapHub;
using namespace lapCore;

HubApp::HubApp(Project &project) : App(project)
{}

void HubApp::Init()
{
    InitWindow(800, 450, "lapHub");
    SetTargetFPS(60);

    project.main_scene = project.GetMainScene();

    state = AppState::RUNNING;
}

void HubApp::Update(float deltaTime)
{
    if (WindowShouldClose())
    {
        state = AppState::DEAD;
        return;
    }

    project.main_scene->Update(deltaTime, SystemDrawOrder::PREDRAW);
}

void HubApp::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    project.main_scene->Update(0.0f, SystemDrawOrder::DRAW);

    EndDrawing();
}

int main()
{
    Project hubProject = UnpackProject(proj_json);
    HubApp hub(hubProject);

    hub.Init();
    hub.Run();
    hub.Shutdown();

    return 0;
}