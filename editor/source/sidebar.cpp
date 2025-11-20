#include "editor.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapEditor;

namespace SidebarObject {
    void OpenSelectionMenu(Scene* scene, entt::entity entity)
    {
        auto &selectionMenu = scene->nameToEntity["selectionMenu"];
        auto &frame = scene->entities.get<Frame>(selectionMenu);

        Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);

        frame.origin.position.offset = mousePos;
        frame.renderable.visible = true;
    }

    const bool setup = [] {
        ScriptRegistry::onCreateFunctions["setup-sidebar"] = [](Scene* scene, entt::entity entity) {
            ConnectECSEvent(scene, entity, "open-selection-menu", OpenSelectionMenu);
        };
        return true;
    }();
};