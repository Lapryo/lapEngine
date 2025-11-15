#include "editor.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapEditor;

namespace FPSLabelObject {
    const bool setup = [] {
        ScriptRegistry::onUpdateFunctions["updatefps"] = [](Scene* scene, entt::entity entity, float dt) {
            std::string fps = "FPS: " + std::to_string(GetFPS());
            scene->entities.get<TextLabel>(entity).text = fps;
        };
        return true;
    }();
};