#include "app.hpp"
#include "scene.hpp"

#include <iostream>

using namespace lapCore;

lapCore::App::App(Project &project) : world(this, project)
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