#include "core.hpp"
#include "scene.hpp"

using namespace lapCore;

void GUISystem::Update(float deltaTime, entt::registry &registry)
{
    rl::Vector2 mouse = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);

    auto uilistView = registry.view<UIList, Frame>();
    for (auto [entity, list, frame] : uilistView.each())
    {
        auto children = scene->GetChildren(entity);

        if (list.direction == Axis2D::VERTICAL)
        {
            float frameY = frame.origin.size.scale.y * scene->logicalResolution.y + frame.origin.size.offset.y;
            float logicalY = list.scrollSize.scale.y * frameY + list.scrollSize.offset.y;
            float offsetY = -(list.displaySize.scale.y * logicalY + list.displaySize.offset.y); // start above first item

            for (int i = 0; i < (int)children.size(); i++)
            {
                auto e = children[i].object;
                // Use the child's position in the parent vector as the child index.
                // This avoids accidental insertion into scene->objectMap when a slot is empty (name == "").
                int idx = i;
                float o_y = offsetY + (idx + 1) * (list.displaySize.scale.y * logicalY + list.displaySize.offset.y);

                auto *attribute = registry.try_get<Attribute<bool>>(e);
                bool use_visibility = attribute ? attribute->value : true;

                auto *label = registry.try_get<TextLabel>(e);
                if (label)
                {
                    if (use_visibility) label->frame.renderable.visible = frame.renderable.visible;
                    label->frame.origin.position = frame.origin.position;

                    label->frame.origin.position.offset.y += o_y;
                }

                auto *frameComp = registry.try_get<Frame>(e);
                if (frameComp)
                {
                    if (use_visibility) frameComp->renderable.visible = frame.renderable.visible;
                    frameComp->origin.position = frame.origin.position;

                    frameComp->origin.position.offset.y += o_y;
                }

                auto *button = registry.try_get<UIButton>(e);
                if (button)
                {
                    button->bounds.position = frame.origin.position;
                    if (use_visibility) button->active = frame.renderable.visible;

                    button->bounds.position.offset.y += o_y;
                }
            }
        }
        else
        {
            float frameX = frame.origin.size.scale.x * scene->logicalResolution.x + frame.origin.size.offset.x;
            float logicalX = list.scrollSize.scale.x * frameX + list.scrollSize.offset.x;
            float offsetX = -(list.displaySize.scale.x * logicalX + list.displaySize.offset.x); // start next to first item

            for (int i = 0; i < (int)children.size(); i++)
            {
                auto e = children[i].object;
                // Use the vector index as the child index to keep spacing consistent even when there are gaps.
                int idx = i;
                float o_x = offsetX + (idx + 1) * (list.displaySize.scale.x * logicalX + list.displaySize.offset.x);

                auto *attribute = registry.try_get<Attribute<bool>>(e);
                bool use_visibility = attribute ? attribute->value : true;

                auto *label = registry.try_get<TextLabel>(e);
                if (label)
                {
                    if (use_visibility) label->frame.renderable.visible = frame.renderable.visible;
                    label->frame.origin.position = frame.origin.position;

                    label->frame.origin.position.offset.x += o_x;
                }

                auto *frameComp = registry.try_get<Frame>(e);
                if (frameComp)
                {
                    if (use_visibility) frameComp->renderable.visible = frame.renderable.visible;
                    frameComp->origin.position = frame.origin.position;

                    frameComp->origin.position.offset.x += o_x;
                }

                auto *button = registry.try_get<UIButton>(e);
                if (button)
                {
                    button->bounds.position = frame.origin.position;
                    if (use_visibility) button->active = frame.renderable.visible;

                    button->bounds.position.offset.x += o_x;
                }
            }
        }
    }

    auto buttonView = registry.view<UIButton>();
    for (auto &entity : buttonView)
    {
        auto *button = &buttonView.get<UIButton>(entity);
        if (!button || !button->active)
            continue;

        rl::Rectangle rect = UIOriginToRect(button->bounds, scene->logicalResolution.x, scene->logicalResolution.y);
        bool hovered = rl::CheckCollisionPointRec(mouse, rect);

        if (hovered)
        {
            if (rl::IsMouseButtonPressed(rl::MOUSE_LEFT_BUTTON))
                EventRegistry::Fire<>(button->events.events["left-click"]);
            if (rl::IsMouseButtonPressed(rl::MOUSE_RIGHT_BUTTON))
                EventRegistry::Fire<>(button->events.events["right-click"]);
            if (rl::IsMouseButtonPressed(rl::MOUSE_MIDDLE_BUTTON))
                EventRegistry::Fire<>(button->events.events["middle-click"]);

            if (button->mouseHovering == false)
                EventRegistry::Fire<>(button->events.events["mouse-enter"]);

            button->mouseHovering = true;

            EventRegistry::Fire<>(button->events.events["mouse-hover"]);
        }
        else
        {
            if (button->mouseHovering == true)
                EventRegistry::Fire<>(button->events.events["mouse-leave"]);

            button->mouseHovering = false;
        }
    }
}