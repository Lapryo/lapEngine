#include "hub.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapHub;

namespace backgroundObject {
    const bool setup = [] {
        ScriptRegistry::onCreateFunctions["adjustbackground"] = [](Scene* scene, entt::entity entity) {
            auto &rect = scene->entities.get<RectVisualizer>(entity);

            float scale = scene->logicalResolution.x / 6400;
            rect.size.x *= scale;
            rect.size.y *= scale;
        };
        return true;
    }();
};