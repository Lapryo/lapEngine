#include "objects/selectionMenu.hpp"

void SelectionMenuObject::RegisterLogic()
{
    ScriptRegistry::onUpdateFunctions["selectionmenu"] = [](Scene *scene, Object object, float dt)
    {
        auto *frame = scene->FindElement<Frame>(object);
        if (!frame || !frame->renderable.visible)
            return;

        Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            Rectangle bounds = UIOriginToRect(frame->origin, scene->logicalResolution.x, scene->logicalResolution.y);
            if (!CheckCollisionPointRec(mousePos, bounds))
            {
                frame->renderable.visible = false;
            }
        }
    };
}