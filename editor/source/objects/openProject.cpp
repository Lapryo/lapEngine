#include "objects/openProject.hpp"
#include "editor.hpp"

void OpenProject(Scene* scene, Object &object)
{
    std::cout << "open project.\n";
}

void HighlightOpenProjectButton(Scene* scene, Object &object)
{
    auto &frame = scene->FindElement<Frame>(object);
    frame.renderable.tint = (Color){255, 255, 255, 25};
}

void UnhighlightOpenProjectButton(Scene* scene, Object &object)
{
    auto &frame = scene->FindElement<Frame>(object);
    frame.renderable.tint = (Color){255, 255, 255, 0};
}

void OpenProjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-open-project-button"] = [](Scene* scene, Object &object) {
        ConnectECSEvent(scene, object, "open-project", OpenProject);
        ConnectECSEvent(scene, object, "highlight-open-project-button", HighlightOpenProjectButton);
        ConnectECSEvent(scene, object, "unhighlight-open-project-button", UnhighlightOpenProjectButton);
    };
}