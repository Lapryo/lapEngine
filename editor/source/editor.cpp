#include "editor.hpp"

#include <iostream>

using namespace lapCore;
using namespace lapEditor;

Editor::Editor(Project &project) : App(project)
{}

void Editor::Init()
{

}

void Editor::Update(float deltaTime)
{

}

void Editor::Draw()
{

}

int main(int argc, char *argv[])
{
    std::cout << "========== [EDITOR] ==========\n\nEditor application started!\nAttempting to open project file \"" << argv[1] << "\"...\n";
    Project project = UnpackProject(argv[1]);

    Editor editor(project);

    std::cout << "Initializing the project... ";
    editor.Init();
    std::cout << "Done.\n\n";

    Scene main_scene;
    PhysicsSystem physics = PhysicsSystem();
    main_scene.AddSystem(&physics);

    auto entity = main_scene.AddEntity();

    Transform2D transform;
    transform.position = {0.0f, 0.0f};
    transform.velocity = {1.0f, -1.0f};
    transform.scale = {0.0f, 0.0f};
    transform.rotation = 0.0f;

    main_scene.AddComponent<Transform2D>(entity, transform);

    std::cout << "X: " << transform.position.x << " | Y: " << transform.position.y << "\n";

    for (int i = 0; i < 10; ++i)
    {
        main_scene.Update(0);
    }

    std::cout << "got to here 1.\n";

    main_scene.Clear();

    std::cout << "got to here 2.\n";

    project.Clear();

    return 0;
}