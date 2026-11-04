#include "game.hpp"

namespace PrefabChildren
{
    bool Game::Init()
    {
        world.LoadSettings("");
        world.LoadWindow();
        world.SetScene(world.GetMainSceneData());

        auto children = world.mainScene->GetChildren(world.mainScene->root);
        for (const auto& obj : children)
        {
            auto data = world.mainScene->GetObject(obj);
            if (data)
            {
                cout << data->id << '\n';
            }

            if (world.mainScene->FindElement<UIFrame>(obj))
                cout << "True\n";
        }

        return true;
    }

    void Game::Update(float deltaTime)
    {
        world.mainScene->Update(deltaTime, world.window.target);

        if (WindowShouldClose())
            Shutdown();
    }
}

int main()
{
    Project project = LoadProject("assets/project/project.json");
    PrefabChildren::Game app(project);
    app.Run();

    return 0;
}