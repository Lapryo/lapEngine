#include "hub.hpp"
using namespace lapCore;
using namespace lapHub;

int maxFPS = 0;

namespace MaxFPSLabelObject {
    const bool updatemaxfps = [] {
        ScriptRegistry::onUpdateFunctions["updatemaxfps"] = [](Scene* scene, entt::entity entity, float dt) {
            int fps = GetFPS();
            if (fps > maxFPS)
                maxFPS = fps;

            std::string maxFPS_string = std::to_string(maxFPS);
            scene->entities.get<TextLabel>(entity).text = "MAX FPS: " + maxFPS_string;
        };
        return true;
    }();
}