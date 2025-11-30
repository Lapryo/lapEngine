#include "objects/fileButton.hpp"
#include "editor.hpp"

void OpenDropdown(Scene *scene, Object object)
{
    auto fileDropdownObj = scene->FindObject("file-dropdown").info.object;
    auto *frame = scene->FindElement<Frame>(scene->objects, fileDropdownObj);
    auto *button = scene->FindElement<UIButton>(scene->objects, fileDropdownObj);

    if (frame)
        frame->renderable.visible = true;
    if (button)
        button->active = true;
}

void HighlightFileButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 25};
}

void UnhighlightFileButton(Scene *scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(scene->objects, object);
    if (frame)
        frame->renderable.tint = (rl::Color){255, 255, 255, 0};
}

void FileButtonObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-file-button"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "open-file-dropdown", OpenDropdown);
        ConnectECSEvent(scene, object, "highlight-file-button", HighlightFileButton);
        ConnectECSEvent(scene, object, "unhighlight-file-button", UnhighlightFileButton);
    };
}