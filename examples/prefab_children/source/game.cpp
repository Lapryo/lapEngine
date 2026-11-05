#include "game.hpp"

namespace PrefabChildren
{
    bool Game::Init()
    {
        world.LoadSettings("");
        world.LoadWindow();
        world.SetScene(world.GetMainSceneData());

        auto photoFramePrefab = world.prefabs->FindFirstObject(HASH_ID("photo-frame"));
        if (photoFramePrefab == null) return false;

        auto photoFrameData = world.prefabs->GetObject(photoFramePrefab);
        if (!photoFrameData) return false;

        ObjectData newData = *photoFrameData;
        newData.id = HASH_ID("example-3");

        auto newPhotoFrame = world.mainScene->AddPrefab(photoFramePrefab, newData);
        auto frame = world.mainScene->FindElement<UIFrame>(newPhotoFrame);
        
        auto photoObj = world.mainScene->FindFirstChild(newPhotoFrame, HASH_ID("photo"));
        auto caption = world.mainScene->FindFirstChild(photoObj, HASH_ID("caption"));
        auto text = world.mainScene->FindElement<UITextLabel>(caption);

        if (frame)
        {
            frame->transform.position.scale.x = 0.75f;
        }

        if (text)
        {
            text->frame.renderable.tint = {255, 0, 0, 255};
            text->text = "SUPER Fake Mona Lisa";
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