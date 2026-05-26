#include "game.hpp"
#include "project_data.hpp" // Note this
/*
When loading in project data, you have two options to choose from:
1. You can load from a JSON files directly into the project.
2. You can load from a project_data.hpp file, which is essentially the same thing, but the JSON is an all in one string header file.
The first option is great for development, nice for debugging, prototyping, and is generally just easier to work with.
The second option is better for distribution, it prevents others from being able to easily snoop around the project files,
unless they know how to look through the binary for strings.
In this example, we will be using the second option, since this application doesn't need much "prototyping" and is very simple.
*/

namespace BlankWindow
{
    bool Game::Init()
    {
        /*
        Basic things such as defaultly loading the window, settings, even the main scene, must be done by the developer.
        This is because lapEngine is designed to give the most freedom possible to the developer,
        and not make any assumptions on how they want to setup their application.

        If at any point something goes wrong, return false
        */
            
        world.LoadWindow();
        world.SetScene(world.GetMainSceneData());

        // You can also do other things here such as setting script functions, loading assets or using assets,
        // or really anything else you can think of.

        return true; // Return true if initialization was successful
    }

    void Game::Update(float deltaTime)
    {
        /*
        Anything you want to do every frame can go here.
        For example, checking for input to make the window fullscreen, updating the main scene, etc.
        However, you wll most likely main do updates through scripts or other elements,vso it's recommended to keep this section
        clean and only put things here that are necessary to be done every frame and aren't easily done through other means.
        */

        // For us, we simply update the main scene
        world.mainScene->Update(deltaTime, world.window.target);
    }
}

int main()
{
    /*
    Here is where you actually load the project and create the Game application, then run it.
    Note here that we are loading the project from the proj_data string in the project_data.hpp file.
    */

    Project project = UnpackProject(proj_data);
    BlankWindow::Game app(project);
    app.Run();

    return 0;
}