#include "objects/fileButton.hpp"
#include "editor.hpp"

void OpenDropdown(Scene* scene, Object &object)
{
    std::cout << "open file drop down.\n";

    auto fileDropdown = scene->FindObject("file-dropdown");
    auto &frame = scene->FindElement<Frame>(fileDropdown);
    auto &button = scene->FindElement<UIButton>(fileDropdown);

    auto openProjObject = scene->FindObject("open-project-button");
    auto &openProjText = scene->FindElement<TextLabel>(openProjObject);
    auto &openProjButton = scene->FindElement<UIButton>(openProjObject);
    auto &openProjFrame = scene->FindElement<Frame>(openProjObject);

    frame.renderable.visible = true;
    button.active = true;
    openProjButton.active = true;
    openProjText.frame.renderable.visible = true;
    openProjFrame.renderable.visible = true;
}

void HighlightFileButton(Scene* scene, Object &object)
{
    auto &frame = scene->FindElement<Frame>(object);
    frame.renderable.tint = (Color){255, 255, 255, 25};
}

void UnhighlightFileButton(Scene* scene, Object &object)
{
    auto &frame = scene->FindElement<Frame>(object);
    frame.renderable.tint = (Color){255, 255, 255, 0};
}

void FileButtonObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-file-button"] = [](Scene* scene, Object &object) {
        ConnectECSEvent(scene, object, "open-file-dropdown", OpenDropdown);
        ConnectECSEvent(scene, object, "highlight-file-button", HighlightFileButton);
        ConnectECSEvent(scene, object, "unhighlight-file-button", UnhighlightFileButton);
    };
}