#include "core.hpp"

using namespace lapCore;

void ApplyAxisOffset(UIOrigin &origin, Axis2D axis, float offset)
{
    if (axis == Axis2D::HORIZONTAL)
        origin.gui.position.x += offset;
    else
        origin.gui.position.y += offset;
}

void GUISystem::ResetInUIList()
{
    auto listView = scene->objects.view<UIList>();
    for (auto [e, list] : listView.each())
    {
        auto obj = scene->FindEntry(e);
        if (obj)
        {
            const auto &children = obj->children;
            for (const auto &child : children)
            {
                const auto& childObj = child.second.object;
                auto* text = scene->objects.try_get<UITextLabel>(childObj);
                auto* frame = scene->objects.try_get<UIFrame>(childObj);
                auto* button = scene->objects.try_get<UIButton>(childObj);

                if (text) text->frame.renderable.inUIList = false;
                if (frame) frame->renderable.inUIList = false;
                if (button) button->inUIList = false;
            }
        }
    }
}

// I quite literally have no idea how this works or how I managed to make this work
void ArrangeUIListElements(Scene *scene, entt::registry &registry)
{
    auto uilistView = registry.view<UIList, UIFrame>();
    for (auto [entity, list, frame] : uilistView.each())
    {
        auto obj = scene->FindEntry(entity);
        auto children = obj->children;

        Rectangle frameTransform = frame.renderable.drawRect;

        Vector2 scrollSize = FrameVectorToVec2(list.scrollSize, {frameTransform.width, frameTransform.height});
        Vector2 displaySize = FrameVectorToVec2(list.displaySize, scrollSize);

        auto anchorVec = FrameVectorToVec2(frame.origin.transform.anchor, scrollSize);
        frameTransform.x -= anchorVec.x;
        frameTransform.y -= anchorVec.y;

        float step = (list.settings.direction == Axis2D::VERTICAL) ? displaySize.y : displaySize.x;

        for (size_t i = 0; i < children.size(); i++)
        {
            auto e = children[i].object;
            float offset = i * step + list.scrollOffset;

            auto *label = registry.try_get<UITextLabel>(e);
            if (label)
            {
                label->frame.renderable.inUIList = true;
                label->frame.renderable.visible = label->frame.renderable.usesUIListVisiblity && frame.renderable.visible;

                Vector2 pos = (list.settings.direction == Axis2D::VERTICAL) ? Vector2{frameTransform.x, frameTransform.y - step} : Vector2{frameTransform.x - step, frameTransform.y};
                label->frame.origin.gui.position = pos;
                label->frame.origin.gui.size = FrameVectorToVec2(label->frame.origin.transform.size, displaySize);

                ApplyAxisOffset(label->frame.origin, list.settings.direction, offset);
            }

            auto *frameComp = registry.try_get<UIFrame>(e);
            if (frameComp)
            {
                frameComp->renderable.inUIList = true;
                frameComp->renderable.visible = frameComp->renderable.usesUIListVisiblity && frame.renderable.visible;
                Vector2 pos = (list.settings.direction == Axis2D::VERTICAL) ? Vector2{frameTransform.x, frameTransform.y - step} : Vector2{frameTransform.x - step, frameTransform.y};
                frameComp->origin.gui.position = pos;
                frameComp->origin.gui.size = FrameVectorToVec2(frameComp->origin.transform.size, displaySize);

                ApplyAxisOffset(frameComp->origin, list.settings.direction, offset);
            }

            auto *button = registry.try_get<UIButton>(e);
            if (button)
            {
                button->inUIList = true;
                button->active = button->inheritsListVisibility && frame.renderable.visible;
                Vector2 pos = (list.settings.direction == Axis2D::VERTICAL) ? Vector2{frameTransform.x, frameTransform.y - step} : Vector2{frameTransform.x - step, frameTransform.y};
                button->bounds.gui.position = pos;
                button->bounds.gui.size = FrameVectorToVec2(button->bounds.transform.size, displaySize);

                ApplyAxisOffset(button->bounds, list.settings.direction, offset);
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

        Rectangle rect = UIOriginToRect(button->bounds, scene->world->window.logical_resolution);
        bool hovered = CheckCollisionPointRec(GetMouseInViewportSpace(scene->world->window.logical_resolution), rect);

        if (hovered)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                EventRegistry::Fire(scene, button->eventCallbacks.leftClick);
            }

            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                EventRegistry::Fire(scene, button->eventCallbacks.rightClick);
            }

            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
            {
                EventRegistry::Fire(scene, button->eventCallbacks.middleClick);
            }

            if (button->mouseHovering == false)
            {
                EventRegistry::Fire(scene, button->eventCallbacks.mouseEnter);
            }

            button->mouseHovering = true;

            EventRegistry::Fire(scene, button->eventCallbacks.mouseHover);
        }
        else
        {
            if (button->mouseHovering == true)
            {
                EventRegistry::Fire(scene, button->eventCallbacks.mouseExit);
            }

            button->mouseHovering = false;
        }
    }
}

void HandleUIListScroll(float deltaTime, Scene *scene, entt::registry &registry)
{
    float wheel = GetMouseWheelMove();
    auto uilistView = registry.view<UIList, UIFrame>();
    for (auto [entity, list, frame] : uilistView.each())
    {
        if (wheel != 0 && CheckCollisionPointRec(GetMouseInViewportSpace(scene->world->window.logical_resolution), UIOriginToRect(frame.origin, scene->world->window.logical_resolution)))
        {
            list.scrollOffset -= wheel * list.settings.scrollSpeed * deltaTime * 1000.0f;

            Vector2 frameSize = {frame.renderable.drawRect.width, frame.renderable.drawRect.height};
            Vector2 scrollSize = FrameVectorToVec2(list.scrollSize, {frameSize.x, frameSize.y});
            Vector2 displaySize = FrameVectorToVec2(list.displaySize, scrollSize);

            list.scrollOffset = std::clamp(list.scrollOffset, 0.0f, (list.settings.direction == Axis2D::VERTICAL) ? scrollSize.y - displaySize.y : scrollSize.x - displaySize.x);
        }
    }
}

void GUISystem::Update(float deltaTime, entt::registry &registry)
{
    HandleUIListScroll(deltaTime, scene, registry);
    ArrangeUIListElements(scene, registry);
    HandleButtonInputs(scene, registry);
}