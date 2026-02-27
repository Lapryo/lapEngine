#include "game.hpp"
#include "project_data.hpp"

// Constructor for the application, takes in the project to run
BlankWindow::BWApp::BWApp(lapCore::Project &project) : App(project)
{
}

bool BlankWindow::BWApp::Init()
{
    world.LoadSettings("");
    world.LoadWindow();

    Project project = world.GetProject();
    world.SetScene(project.scenes[project.main_scene_index]);

    return true;
}

void BlankWindow::BWApp::Update(float deltaTime)
{
    world.main_scene.Update(deltaTime, world.window.target);
}

int main()
{
    Project bwProject = UnpackProject(json_file);
    BlankWindow::BWApp app(bwProject);
    app.Run();

    return 0;
}