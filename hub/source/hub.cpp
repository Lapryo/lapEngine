#include "hub.hpp"
#include "projdata.hpp"
#include <iostream>

using namespace lapHub;
using namespace lapCore;

HubApp::HubApp(Project &project) : App(project)
{}

entt::entity fpsntt;
entt::entity cam;
entt::entity box;

const int screen_width = 800;
const int screen_height = 800;

void HubApp::Init()
{
    InitWindow(screen_width, screen_height, "lapHub");
    SetTargetFPS(-1);

    project.main_scene = project.GetMainScene();
    fpsntt = project.main_scene->FindEntity("fpslabel");
    cam = project.main_scene->FindEntity("camera");
    box = project.main_scene->FindEntity("box");

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

    Vector2 boxPos = project.main_scene->entities.get<Transform2D>(box).position;
    boxPos.x -= screen_width/2;
    boxPos.y -= screen_height/2;

    Camera2D& camTarget = project.main_scene->entities.get<Cam2D>(cam).camera;
    const float LERP_SMOOTHING = 0.5f;

    camTarget.target.x += (boxPos.x - camTarget.target.x) * LERP_SMOOTHING * deltaTime;
    camTarget.target.y += (boxPos.y - camTarget.target.y) * LERP_SMOOTHING * deltaTime;

    static float timer = 0.0f;
    timer += deltaTime;
    if (timer >= 5.0f)
    {
        project.main_scene->entities.get<Transform2D>(box).velocity.y -= 10.0f;
        timer = 0.0f;
    }
}

void HubApp::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    std::string fps = std::to_string(GetFPS());
    project.main_scene->entities.get<TextLabel>(fpsntt).text = "FPS: " + fps;

    project.main_scene->Update(0.0f, SystemDrawOrder::DRAW);

    /*
        TODO:
            ADD ASSET CREATION QUEUE FOR PROJECT (after window is initiated)
            GET SPRITES WORKING
            OPTIMIZE
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