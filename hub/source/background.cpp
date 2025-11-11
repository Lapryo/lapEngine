#include "hub.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapHub;

namespace backgroundObject {
    const bool setup = [] {
        ScriptRegistry::onCreateFunctions["adjustbackground"] = [](Scene* scene, entt::entity entity) {
            auto &rect = scene->entities.get<RectVisualizer>(entity);

            rect.size.x *= scene->resolutionScale;
            rect.size.y *= scene->resolutionScale;
        };
        return true;
    }();
};