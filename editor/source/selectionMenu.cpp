#include "editor.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapEditor;

namespace SelectionMenuObject {
    const bool setup = [] {
        ScriptRegistry::onUpdateFunctions["selectionmenu"] = [](Scene* scene, entt::entity entity, float dt) {
            auto& frame = scene->entities.get<Frame>(entity);

            Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                Rectangle bounds = UIOriginToRect(frame.origin, scene->logicalResolution.x, scene->logicalResolution.y);
                if (!CheckCollisionPointRec(mousePos, bounds))
                {
                    frame.renderable.visible = false;
                }
            }
        };
        return true;
    }();
};