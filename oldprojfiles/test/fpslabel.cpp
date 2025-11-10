#include "hub.hpp"
using namespace lapCore;
using namespace lapHub;

namespace FPSLabelObject {
    const bool updatefps = [] {
        ScriptRegistry::onUpdateFunctions["updatefps"] = [](Scene* scene, entt::entity entity, float dt) {
            std::string fps = std::to_string(GetFPS());
            scene->entities.get<TextLabel>(entity).text = "FPS: " + fps;
        };
        return true;
    }();
}