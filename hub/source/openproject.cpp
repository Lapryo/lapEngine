#include "hub.hpp"
#include <iostream>

using namespace lapCore;
using namespace lapHub;

namespace opProjectObject {
    Color highlight = {255, 255, 255, 100};
    Color unhighlight = {255, 255, 255, 0};

    const bool setup = [] {
        ScriptRegistry::onCreateFunctions["adjustopenproject"] = [](Scene* scene, entt::entity entity) {
            auto &transform = scene->entities.get<Transform2D>(entity);
            auto &label = scene->entities.get<TextLabel>(entity);

            label.textSize *= scene->resolutionScale;

            float textWidth = MeasureText(label.text.c_str(), label.textSize);

            auto &hitbox = scene->entities.get<RectVisualizer>(entity);
            hitbox.size = {textWidth + 60 * (float)scene->resolutionScale, label.textSize + 40 * (float)scene->resolutionScale};
            hitbox.offset.x *= scene->resolutionScale;
            hitbox.offset.y *= scene->resolutionScale;

            float halfLogicalWidth = scene->logicalResolution.x / 2;

            Vector2 targetPosition = {
                scene->logicalResolution.x - halfLogicalWidth / 2 - textWidth / 2,
                scene->logicalResolution.y / 2 - label.textSize * 1.5f
            };

            transform.position = targetPosition;

            EventRegistry::Connect<>(
                "onOpenProject",
                []()
                {
                    std::cout << "Move to project list scene.\n";
                }
            );
        };

        ScriptRegistry::onUpdateFunctions["openproject"] = [](Scene* scene, entt::entity entity, float dt) {
            bool inRange = false;

            Vector2 mousePos = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);
            auto &t = scene->entities.get<Transform2D>(entity);

            auto &hitbox = scene->entities.get<RectVisualizer>(entity);

            Vector2 transform = {t.position.x + hitbox.offset.x, t.position.y + hitbox.offset.y};

            if (mousePos.x >= transform.x && mousePos.x <= transform.x + hitbox.size.x && mousePos.y >= transform.y && mousePos.y <= transform.y + hitbox.size.y)
            {
                hitbox.tint = highlight;
                inRange = true;
            }
            else
                hitbox.tint = unhighlight;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && inRange)
                EventRegistry::Fire<>("onOpenProject");
        };
        return true;
    }();
};