#include "hub.hpp"
#include "projdata.hpp"
#include <iostream>

using namespace lapHub;
using namespace lapCore;

HubApp::HubApp(Project &project) : App(project)
{}

void HubApp::Init()
{
    InitWindow(1920, 1080, "lapHub");
    SetTargetFPS(-1);

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

    /*
        TODO:
        ADD CAMERA DRAW ORDER
        ADD Z LAYERS
    */

    EndDrawing();

    project.main_scene->Update(0.0f, SystemDrawOrder::POSTDRAW);
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