#include "objects/newProject.hpp"
#include "editor.hpp"

void NewProject(Scene *scene, Object object)
{
    std::cout << "Creating new project...\n";

    std::string filePath = FileDialogs::SaveFile({"lapEngine Project Files", "*.lapengine"});
    if (filePath.empty())
        return;

    lapEditor::loadedProjectFilePath = filePath;
    lapEditor::loadedProjectJson = {
        {"name", "Untitled Project"},
        {"version", "1.0.0"},
        {"scenes", nlohmann::json::array()}
    };

    auto saveProjButtonObj = scene->FindObject("save-project-button").info.object;
    auto *saveProjButtonVisibilityAttr = scene->FindElement<Attribute<bool>>(scene->objects, saveProjButtonObj);
    if (saveProjButtonVisibilityAttr)
        saveProjButtonVisibilityAttr->value = true;

    auto closeProjBtnObj = scene->FindObject("close-project-button").info.object;
    auto *closeProjBtnVisibilityAttr = scene->FindElement<Attribute<bool>>(scene->objects, closeProjBtnObj);
    if (closeProjBtnVisibilityAttr)
        closeProjBtnVisibilityAttr->value = true;

    EventRegistry::Fire<>("refresh-sidebar");
}

void HighlightNewProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 25};
}

void UnhighlightNewProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 0};
}

void NewProjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-new-project-button"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "new-project", NewProject);
        ConnectECSEvent(scene, object, "highlight-new-project-button", HighlightNewProjectButton);
        ConnectECSEvent(scene, object, "unhighlight-new-project-button", UnhighlightNewProjectButton);
    };
}