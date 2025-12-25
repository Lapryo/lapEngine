#include "core.hpp"
#include "scene.hpp"

using namespace lapCore;

void ApplyAxisOffset(UIOrigin &origin, Axis2D axis, float offset)
{
    if (axis == Axis2D::HORIZONTAL)
        origin.position.offset.x += offset;
    else
        origin.position.offset.y += offset;
}

void ArrangeUIListElements(Scene *scene, entt::registry &registry)
{
    auto uilistView = registry.view<UIList, Frame>();
    for (auto [entity, list, frame] : uilistView.each())
    {
        auto children = scene->GetChildren(entity);

        rl::Vector2 frameSize = FrameVectorToVec2(frame.origin.size, scene->logicalResolution.x, scene->logicalResolution.y);
        rl::Vector2 scrollSize = FrameVectorToVec2(list.scrollSize, frameSize.x, frameSize.y);
        rl::Vector2 displaySize = FrameVectorToVec2(list.displaySize, frameSize.x, frameSize.y);
        float step = (list.direction == Axis2D::VERTICAL) ? displaySize.y : displaySize.x;

        for (size_t i = 0; i < children.size(); i++)
        {
            auto e = children[i].object;

            float offset = i * step + list.scrollOffset;

            auto *label = registry.try_get<TextLabel>(e);
            if (label)
            {
                label->frame.renderable.visible = label->frame.renderable.usesUIListVisiblity && frame.renderable.visible;
                label->frame.origin.position = frame.origin.position;

                ApplyAxisOffset(label->frame.origin, list.direction, offset);
            }

            auto *frameComp = registry.try_get<Frame>(e);
            if (frameComp)
            {
                frameComp->renderable.visible = frameComp->renderable.usesUIListVisiblity && frame.renderable.visible;
                frameComp->origin.position = frame.origin.position;

                ApplyAxisOffset(frameComp->origin, list.direction, offset);
            }

            auto *button = registry.try_get<UIButton>(e);
            if (button)
            {
                button->active = button->usesListVisibility && frame.renderable.visible;
                button->bounds.position = frame.origin.position;

                ApplyAxisOffset(button->bounds, list.direction, offset);
            }
        }
    }
}

void HandleButtonInputs(Scene *scene, entt::registry &registry)
{
    auto buttonView = registry.view<UIButton>();
    for (auto &entity : buttonView)
    {
        auto *button = &buttonView.get<UIButton>(entity);
        if (!button || !button->active)
            continue;

        rl::Rectangle rect = UIOriginToRect(button->bounds, scene->logicalResolution.x, scene->logicalResolution.y);
        bool hovered = rl::CheckCollisionPointRec(GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y), rect);

        if (hovered)
        {
            if (rl::IsMouseButtonPressed(rl::MOUSE_LEFT_BUTTON))
            {
                auto it = button->events.events.find("left-click");
                if (it != button->events.events.end())
                    EventRegistry::Fire<>(it->second);
            }

            if (rl::IsMouseButtonPressed(rl::MOUSE_RIGHT_BUTTON))
            {
                auto it = button->events.events.find("right-click");
                if (it != button->events.events.end())
                    EventRegistry::Fire<>(it->second);
            }

            if (rl::IsMouseButtonPressed(rl::MOUSE_MIDDLE_BUTTON))
            {
                auto it = button->events.events.find("middle-click");
                if (it != button->events.events.end())
                    EventRegistry::Fire<>(it->second);
            }

            if (button->mouseHovering == false)
            {
                auto it = button->events.events.find("mouse-enter");
                if (it != button->events.events.end())
                    EventRegistry::Fire<>(it->second);
            }

            button->mouseHovering = true;

            auto it = button->events.events.find("mouse-hover");
            if (it != button->events.events.end())
                EventRegistry::Fire<>(it->second);
        }
        else
        {
            if (button->mouseHovering == true)
            {
                auto it = button->events.events.find("mouse-leave");
                if (it != button->events.events.end())
                    EventRegistry::Fire<>(it->second);
            }

            button->mouseHovering = false;
        }
    }
}

void HandleUIListScroll(float deltaTime, Scene *scene, entt::registry &registry)
{
    float wheel = rl::GetMouseWheelMove();
    auto uilistView = registry.view<UIList, Frame>();
    for (auto [entity, list, frame] : uilistView.each())
    {
        if (wheel != 0 && rl::CheckCollisionPointRec(GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y), UIOriginToRect(frame.origin, scene->logicalResolution.x, scene->logicalResolution.y)))
        {
            list.scrollOffset -= wheel * list.scrollSpeed * deltaTime * 1000.0f;
            std::cout << list.scrollOffset << std::endl;

            rl::Vector2 frameSize = FrameVectorToVec2(frame.origin.size, scene->logicalResolution.x, scene->logicalResolution.y);
            rl::Vector2 scrollSize = FrameVectorToVec2(list.scrollSize, frameSize.x, frameSize.y);
            rl::Vector2 displaySize = FrameVectorToVec2(list.displaySize, frameSize.x, frameSize.y);

            list.scrollOffset = std::clamp(list.scrollOffset, 0.0f, (list.direction == Axis2D::VERTICAL) ? scrollSize.y - displaySize.y : scrollSize.x - displaySize.x);
        }
    }
}

void GUISystem::Update(float deltaTime, entt::registry &registry)
{
    HandleUIListScroll(deltaTime, scene, registry);
    ArrangeUIListElements(scene, registry);
    HandleButtonInputs(scene, registry);
}