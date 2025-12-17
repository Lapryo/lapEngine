#include "objects/sceneButton.hpp"

void OpenSceneDropdown(Scene* scene, Object object)
{
    std::cout << "open scene dropdown.\n";
}

void HighlightSceneButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = rl::Color{255, 255, 255, 25};
}

void UnhighlightSceneButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = rl::Color{255, 255, 255, 0};
}

void SceneButtonObject::RegisterLogic()
{
	ScriptRegistry::onCreateFunctions["setup-scene-button"] = [](Scene *scene, Object object)
	{
		ConnectECSEvent(scene, object, "open-scene-dropdown", OpenSceneDropdown);
		ConnectECSEvent(scene, object, "highlight-scene-button", HighlightSceneButton);
        ConnectECSEvent(scene, object, "unhighlight-scene-button", UnhighlightSceneButton);
	};
}
