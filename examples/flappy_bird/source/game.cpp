#include "game.hpp"

#include "proj_data.hpp" // THIS IS IMPORTANT!
// The proj_data.hpp file contains everything that is in the project in a string format
// This prevents others from being able to easily snoop around the project files when you distribute your game, making it easy to find exploits or assets you may not want to share freely
// If you're okay with this, you can simply load the project from a separate file instead of including it like this
// Since we already did this for the pong application, we will instead load from a file in this example

/*
TODO:
1. Finish assets for the game
2. Implement the logic for the game
3. Comment everything so other developers can look at this as a basis for their games/apps
*/

// Constructor for the application, takes in the project to run
FlappyBird::FBApp::FBApp(lapCore::Project &project) : App(project)
{}

bool FlappyBird::FBApp::Init()
{
    // Anything specific to Flappy Bird initialization can go here (registering logic, loading settings, etc.)

    // lapEngine has a specific function for loading settings for the window
    world.LoadSettings("assets/settings.json");

    // Anything else you may want to do is recommended to be done between these two sections of initialization

    // This is a good spot to register logic for any objects you may want
    // Since this is flappy bird, we register logic for the pipes and the bird here

    // ...

    return true; // Return true if initialization was successful, false otherwise
}

void FlappyBird::FBApp::Update(float deltaTime)
{
    // Anything you want to do every frame can go here
    // For example, checking for input to make the window fullscreen, updating the main scene, etc.
    // However, you wll most likely main do updates through scripts or other elements, so it's recommended to keep this section clean and only put things here that are necessary to be done every frame and aren't easily done through other means

    // For us, we simply update the main scene
    world.main_scene->Update(deltaTime, world.target);
}

int main()
{
    // Our actual main function, where we create the project and run the application

    Project fbProject = UnpackProject("assets/project/project.json"); /* Remember earlier? We are going to read a file into a string and unpack it here
                                                                                           You must also do the same that you did with the settings file to the project file */
    // lapEngine contains many useful functions like this to just make development easier so you can focus on creating rather than boilerplate code
    FlappyBird::FBApp app(fbProject); // Create the application with the project
    app.Run(); // Now call run on the application, not the update function you have!
    // We do this because the base App class has a run loop that handles window closing and delta time calculation for us

    return 0;
}