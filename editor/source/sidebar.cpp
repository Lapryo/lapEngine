#include "editor.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapEditor;

namespace SidebarObject {
    void OpenSelectionMenu(Scene* scene, entt::entity entity)
    {
        
    }

    const bool setup = [] {
        ScriptRegistry::onCreateFunctions["setup-sidebar"] = [](Scene* scene, entt::entity entity) {
            ConnectECSEvent(scene, entity, "open-selection-menu", OpenSelectionMenu)
        };
        return true;
    }();
};