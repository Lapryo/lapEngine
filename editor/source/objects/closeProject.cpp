#include "objects/closeProject.hpp"
#include "editor.hpp"

void CloseProject(Scene* scene, Object object)
{
    // TODO: Implement closing projects
    std::cout << "closing project.\n";

    lapEditor::loadedProjectJson = {};
    EventRegistry::Fire<>("refresh-sidebar");

    auto saveProjButtonObj = scene->FindObject("save-project-button").info.object;
    auto *saveProjButtonVisibilityAttr = scene->FindElement<Attribute<bool>>(scene->objects, saveProjButtonObj);
    if (saveProjButtonVisibilityAttr)
        saveProjButtonVisibilityAttr->value = false;

    auto closeProjBtnObj = scene->FindObject("close-project-button").info.object;
    auto *closeProjBtnVisibilityAttr = scene->FindElement<Attribute<bool>>(scene->objects, closeProjBtnObj);
    if (closeProjBtnVisibilityAttr)
        closeProjBtnVisibilityAttr->value = false;

    auto sidebarObj = scene->FindObject("sidebar").info.object;
    auto *sidebarFrame = scene->FindElement<Frame>(scene->objects, sidebarObj);
    if (sidebarFrame)
        sidebarFrame->renderable.visible = false;
}

void HighlightCloseProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 25};
}

void UnhighlightCloseProjectButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 0};
}

void CloseProjectObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-close-project-button"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "close-project", CloseProject);
        ConnectECSEvent(scene, object, "highlight-close-project-button", HighlightCloseProjectButton);
        ConnectECSEvent(scene, object, "unhighlight-close-project-button", UnhighlightCloseProjectButton);
    };
}