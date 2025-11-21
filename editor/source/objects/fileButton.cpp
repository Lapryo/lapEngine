#include "objects/fileButton.hpp"
#include "editor.hpp"

void OpenDropdown(Scene* scene, Object object)
{
    auto fileDropdown = scene->FindObject("file-dropdown");
    auto *frame = scene->FindElement<Frame>(fileDropdown);
    auto *button = scene->FindElement<UIButton>(fileDropdown);

    if (frame)
        frame->renderable.visible = true;
    if (button)
        button->active = true;
}

void HighlightFileButton(Scene* scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(object);
    if (frame)
        frame->renderable.tint = (Color){255, 255, 255, 25};
}

void UnhighlightFileButton(Scene* scene, Object object)
{
    auto *frame = scene->FindElement<Frame>(object);
    if (frame)
        frame->renderable.tint = (Color){255, 255, 255, 0};
    else
        std::cout << "for some reason, the file button doesn't think it has a frame?\n";
}

void FileButtonObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-file-button"] = [](Scene* scene, Object object) {
        ConnectECSEvent(scene, object, "open-file-dropdown", OpenDropdown);
        ConnectECSEvent(scene, object, "highlight-file-button", HighlightFileButton);
        ConnectECSEvent(scene, object, "unhighlight-file-button", UnhighlightFileButton);
    };
}