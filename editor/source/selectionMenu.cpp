#include "editor.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapEditor;

namespace SelectionMenuObject {
    const bool setup = [] {
        ScriptRegistry::onUpdateFunctions["selectionmenu"] = [](Scene* scene, entt::entity entity, float dt) {
            auto& frame = scene->entities.get<Frame>(entity);

            Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);
            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
            {
                auto view = scene->entities.view<Frame, Attribute<bool>>();

                // change this to grab the one highest on top
                for (auto [entity, otherframe, boolAttrib] : view.each())
                {
                    Rectangle bounds = UIOriginToRect(otherframe.origin, scene->logicalResolution.x, scene->logicalResolution.y);
                    if (boolAttrib.name == "opensSelectionMenu" && boolAttrib.value == true && CheckCollisionPointRec(mousePos, bounds))
                    {
                        frame.origin.position.offset = mousePos;
                        frame.renderable.visible = true;
                        break;
                    }
                }
            }
            else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
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