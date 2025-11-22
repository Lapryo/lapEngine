#include "objects/selectionMenu.hpp"

void SelectionMenuObject::RegisterLogic()
{
    ScriptRegistry::onUpdateFunctions["selectionmenu"] = [](Scene *scene, Object object, float dt)
    {
        auto *frame = scene->FindElement<Frame>(object);
        if (!frame || !frame->renderable.visible)
            return;

        rl::Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);
        if (rl::IsMouseButtonDown(rl::MOUSE_LEFT_BUTTON))
        {
            rl::Rectangle bounds = UIOriginToRect(frame->origin, scene->logicalResolution.x, scene->logicalResolution.y);
            if (!rl::CheckCollisionPointRec(mousePos, bounds))
            {
                frame->renderable.visible = false;
            }
        }
    };
}