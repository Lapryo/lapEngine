#include "hub.hpp"
#include "projdata.hpp"
#include <iostream>
#include <random>

#include "json.hpp"

using namespace lapHub;
using namespace lapCore;

std::vector<std::string> windowSubTitles = {
    "For the love of the game.",
    "Don't feed the bugs!",
    "I swear it works.",
    "Mostly harmless!",
    "We ball.",
    "Don't hate the player, hate the game.",
    "Blame the coder.",
    "Lag, errors, and bugs are part of the experience!",
    "Ctrl+Z is your bsf!",
    "Wonky, but loveable."
};

HubApp::HubApp(Project &project) : App(project)
{}

void HubApp::Init()
{
    project.LoadSettings("assets/settings.json");

    std::random_device rd;
    std::mt19937 rng(rd());

    std::uniform_int_distribution<int> intDist(0, windowSubTitles.size() - 1);
    std::string windowTitle = "lapHub - " + windowSubTitles[intDist(rng)];

    SetWindowTitle(windowTitle.c_str());

    state = AppState::RUNNING;
}

void HubApp::Update(float deltaTime)
{
    if (WindowShouldClose())
    {
        state = AppState::DEAD;
        return;
    }

    if (IsKeyReleased(KEY_F11)) {
        ToggleFullscreen();
        SetWindowPosition(100, 100);
    }

    project.main_scene->Update(deltaTime, SystemDrawOrder::PREDRAW, project.target);
}

void HubApp::Draw()
{
    project.main_scene->Update(0.0f, SystemDrawOrder::DRAW, project.target);
    project.main_scene->Update(0.0f, SystemDrawOrder::POSTDRAW, project.target);
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

/*
    TODO:
    IMPLEMENT GUI SYSTEM
    IMPLEMENT SCALING FOR RESOLUTIONS

    NOTES:
    FPS LAG STEMS FROM LOGICAL RESOLUTION
*/