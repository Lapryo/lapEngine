#include "objects/openProject.hpp"
#include "editor.hpp"

void OpenProject(Scene *scene, Object object)
{
    std::string filePath = FileDialogs::OpenFile({"lapEngine Project Files", "*.lapengine"});
    if (filePath.empty())
        return;

    std::cout << filePath << "\n";
}

void HighlightOpenProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(object);
    if (frame)
        frame->renderable.tint = (Color){255, 255, 255, 25};
}

void UnhighlightOpenProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(object);
    if (frame)
        frame->renderable.tint = (Color){255, 255, 255, 0};
}

void OpenProjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-open-project-button"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "open-project", OpenProject);
        ConnectECSEvent(scene, object, "highlight-open-project-button", HighlightOpenProjectButton);
        ConnectECSEvent(scene, object, "unhighlight-open-project-button", UnhighlightOpenProjectButton);
    };
}