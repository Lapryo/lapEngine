#include "objects/saveProject.hpp"
#include "editor.hpp"

void SaveProject(Scene *scene, Object object)
{
    std::string filePath = FileDialogs::SaveFile({"lapEngine Project Files", "*.lapengine"});
    if (filePath.empty())
        return;

    lapEditor::SaveProjectToFile(filePath);
}

void HighlightSaveProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 25};
}

void UnhighlightSaveProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 0};
}

void SaveProjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-save-project-button"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "save-project", SaveProject);
        ConnectECSEvent(scene, object, "highlight-save-project-button", HighlightSaveProjectButton);
        ConnectECSEvent(scene, object, "unhighlight-save-project-button", UnhighlightSaveProjectButton);
    };
}