#include "app.hpp"
#include "scene.hpp"

#include <iostream>

using namespace lapCore;

lapCore::App::App(Project &project) : world(this, project)
{}

void ResetLogLevel(DebugLevel level)
{
    switch (level)
    {
        case DebugLevel::NONE:
            SetTraceLogLevel(7);
            break;
        case DebugLevel::LOW:
            SetTraceLogLevel(5);
            break;
        case DebugLevel::MEDIUM:
            SetTraceLogLevel(4);
            break;
        case DebugLevel::HIGH:
            SetTraceLogLevel(0);
            break;
    }
}

void App::Run()
{
    ResetLogLevel(DEBUG_LEVEL);

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
        ResetLogLevel(DEBUG_LEVEL);

        Update(GetFrameTime());
        if (world.switchingScene && world.nextSceneData.name != "")
        {
            world.SetScene(world.nextSceneData);
            world.switchingScene = false;
            world.nextSceneData.Clear();
        }
    }
}

void App::Shutdown()
{
    if (state == AppState::DEAD) // If the app is already dead, do nothing
        return;
    
    delete world.mainScene;
    delete world.prefabs;

    world.resources.ClearAll();

    UnloadRenderTexture(world.window.target);
    CloseAudioDevice();

    // shutdown window if it exists
    CloseWindow();

    state = AppState::DEAD;
}