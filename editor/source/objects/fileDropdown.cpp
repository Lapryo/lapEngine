#include "objects/fileDropdown.hpp"
#include "editor.hpp"

void CloseDropdown(Scene* scene, Object &object)
{
    std::cout << "close file drop down.\n";

    auto fileDropdown = scene->FindObject("file-dropdown");
    auto &frame = scene->FindElement<Frame>(fileDropdown);
    auto &button = scene->FindElement<UIButton>(fileDropdown);

    auto openProjObject = scene->FindObject("open-project-button");
    auto &openProjText = scene->FindElement<TextLabel>(openProjObject);
    auto &openProjButton = scene->FindElement<UIButton>(openProjObject);
    auto &openProjFrame = scene->FindElement<Frame>(openProjObject);

    frame.renderable.visible = false;
    button.active = false;
    openProjButton.active = false;
    openProjText.frame.renderable.visible = false;
    openProjFrame.renderable.visible = false;
}

void FileDropdownObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-file-dropdown"] = [](Scene* scene, Object &object) {
        ConnectECSEvent(scene, object, "close-file-dropdown", CloseDropdown);
    };
}