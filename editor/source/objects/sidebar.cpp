#include "objects/sidebar.hpp"

void OpenSelectionMenu(Scene* scene, Object &object)
{
    auto selectionMenu = scene->FindObject("selectionMenu");
    auto &frame = scene->FindElement<Frame>(selectionMenu);

    Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);

    frame.origin.position.offset = mousePos;
    frame.renderable.visible = true;
}

void SidebarObject::RegisterLogic()
{
    ScriptRegistry::onCreateFunctions["setup-sidebar"] = [](Scene* scene, Object &object) {
        ConnectECSEvent(scene, object, "open-selection-menu", OpenSelectionMenu);
    };
}