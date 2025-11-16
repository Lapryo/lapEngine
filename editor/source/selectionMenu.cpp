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
                auto view = scene->entities.view<Frame, BoolAttribute>();
                for (auto [entity, otherframe, boolAttrib] : view.each())
                {
                    Rectangle bounds;
                    bounds.x = otherframe.position.scale.x * scene->logicalResolution.x;
                    bounds.y = otherframe.position.scale.y * scene->logicalResolution.y;
                    bounds.width = otherframe.size.scale.x * scene->logicalResolution.x;
                    bounds.height = otherframe.size.scale.y * scene->logicalResolution.y;

                    if (boolAttrib.name == "opensSelectionMenu" && boolAttrib.value == true && CheckCollisionPointRec(mousePos, bounds))
                    {
                        frame.position.offset = mousePos;
                        frame.renderable.visible = true;
                    }
                }
            }
            else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {

                Rectangle bounds;
                bounds.x = frame.position.offset.x;
                bounds.y = frame.position.offset.y;
                bounds.width = frame.size.scale.x * scene->logicalResolution.x;
                bounds.height = frame.size.scale.y * scene->logicalResolution.y;

                if (!CheckCollisionPointRec(mousePos, bounds))
                {
                    frame.renderable.visible = false;
                }
            }
        };
        return true;
    }();
};