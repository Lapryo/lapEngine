#include "objects/fileDropdown.hpp"
#include "editor.hpp"

void CloseDropdown(Scene *scene, Object object)
{
    auto fileDropdown = scene->FindObject("file-dropdown");
    auto *frame = scene->FindElement<Frame>(fileDropdown);
    auto *button = scene->FindElement<UIButton>(fileDropdown);

    if (frame)
        frame->renderable.visible = false;
    if (button)
        button->active = false;
}

void FileDropdownObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-file-dropdown"] = [](Scene *scene, Object object)
    {
        ConnectECSEvent(scene, object, "close-file-dropdown", CloseDropdown);
    };
}