#include "editor.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapEditor;

namespace AddObjectObject {
    void AddObject(Scene* scene, entt::entity entity)
    {
        std::cout << "bring up name enter textbox.\n";
    }

    const bool setup = [] {
        ScriptRegistry::onCreateFunctions["connect-add-object"] = [](Scene* scene, entt::entity entity) {
            ConnectECSEvent(scene, entity, "add-object", AddObject);
        };
        return true;
    }();
};