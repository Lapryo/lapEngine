#include "core.hpp"

#include <iostream>

using namespace lapCore;

void PhysicsSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Origin2D, Physics2D>();
    for (auto [entity, origin, physics] : view.each())
    {
        origin.position.x += physics.velocity.x * deltaTime;
        origin.position.y += physics.velocity.y * deltaTime;
    }
}

void RenderSystem::Connect(entt::registry &registry)
{
    registry.on_update<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<TextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<Image>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<Frame>().connect<&RenderSystem::OnRenderableUpdated>(*this);
}

void RenderSystem::OnRenderableUpdated(entt::registry &registry, entt::entity entity)
{
    needsResort = true;
}

void RenderSystem::RebuildRenderList(entt::registry &registry)
{
    std::cout << "[RENDER] Rebuilding render list.\n";
    renderList.clear();

    auto spriteView = registry.view<Sprite>();
    for (auto e : spriteView)
    {
        const auto &s = spriteView.get<Sprite>(e);
        renderList.push_back({e, s.renderable.zlayer, s.renderable.isScreenSpace, RenderEntry::Type::Sprite});
    }

    auto imageView = registry.view<Image>();
    for (auto e : imageView)
    {
        const auto &s = imageView.get<Image>(e);
        renderList.push_back({e, s.sprite.renderable.zlayer, s.sprite.renderable.isScreenSpace, RenderEntry::Type::Image});
    }

    auto textView = registry.view<TextLabel>();
    for (auto e : textView)
    {
        const auto &t = textView.get<TextLabel>(e);
        renderList.push_back({e, t.renderable.zlayer, t.renderable.isScreenSpace, RenderEntry::Type::Text});
    }

    auto frameView = registry.view<Frame>();
    for (auto e : frameView)
    {
        const auto &t = frameView.get<Frame>(e);
        renderList.push_back({e, t.renderable.zlayer, t.renderable.isScreenSpace, RenderEntry::Type::Rect});
    }

    std::sort(renderList.begin(), renderList.end(), [](const auto &a, const auto &b)
              {
        if (a.isScreenSpace != b.isScreenSpace)
            return !a.isScreenSpace; // world first
        return a.zlayer < b.zlayer; });

    needsResort = false;
}

/*void RenderSystem::Update(float deltaTime, entt::registry& registry)
{
    if (needsResort)
        RebuildRenderList(registry);

    // Separate world-space and screen-space entities
    std::vector<RenderEntry> worldSpace;
    std::vector<RenderEntry> screenSpace;

    for (const auto& entry : renderList)
    {
        if (entry.isScreenSpace)
            screenSpace.push_back(entry);
        else
            worldSpace.push_back(entry);
    }

    // Draw world-space entities per camera
    for (auto [camEntity, cam] : registry.view<Cam2D>().each())
    {
        BeginMode2D(cam.camera);

        for (const auto& entry : worldSpace)
        {
            switch (entry.type)
            {
                case RenderEntry::Type::Sprite:
                {
                    auto* image = registry.try_get<Image>(entry.entity);
                    if (image && image->sprite.renderable.visible && image->sprite.texture)
                    {
                        // Because it's an image, we now check for a frame and adjust the position and size accordingly
                        // If there is no frame, then use the Origin2D
                        // If there is no Origin2D, or any sufficient position and size data, don't render the image

                        auto* frame = registry.try_get<Frame>(entry.entity);
                        auto* origin = registry.try_get<Origin2D>(entry.entity);
                        if (frame)
                        {
                            float x = frame->position.scale.x * scene->logicalResolution.x + frame->position.offset.x;
                            float y = frame->position.scale.y * scene->logicalResolution.y + frame->position.offset.y;

                            float sx = frame->size.scale.x * scene->logicalResolution.x + frame->size.offset.x;
                            float sy = frame->size.scale.y * scene->logicalResolution.y + frame->size.offset.y;

                            if (frame->useOrigin && origin)
                            {
                                // Add origin position to the frame position
                                // Multiply frame size by scale

                                x += origin->position.x;
                                y += origin->position.y;

                                sx *= origin->scale.x;
                                sy *= origin->scale.y;
                            }

                            DrawTexturePro(
                                *image->sprite.texture,
                                {0.0f, 0.0f, (float)image->sprite.texture->width, (float)image->sprite.texture->height},
                                {x, y, sx, sy},
                                image->sprite.anchor,
                                frame->rotation,
                                image->sprite.renderable.tint
                            );
                        }
                        else if (origin)
                        {
                            // Just use origin
                            DrawTexturePro(
                                *image->sprite.texture,
                                {0.0f, 0.0f, (float)image->sprite.texture->width, (float)image->sprite.texture->height},
                                {origin->position.x, origin->position.y, origin->scale.x, origin->scale.y},
                                image->sprite.anchor,
                                origin->rotation,
                                image->sprite.renderable.tint
                            );
                        }

                        // Else do nothing
                    }
                    else
                    {
                        auto* sprite = registry.try_get<Sprite>(entry.entity);
                        if (sprite && sprite->renderable.visible && sprite->texture)
                        {
                            // Because it's a sprite, we don't need to check for a frame, so just render normally

                            auto* origin = registry.try_get<Origin2D>(entry.entity);
                            if (origin)
                            {
                                DrawTexturePro(
                                    *sprite->texture,
                                    {0.0f, 0.0f, (float)image->sprite.texture->width, (float)image->sprite.texture->height},
                                    {origin->position.x, origin->position.y, origin->scale.x, origin->scale.y},
                                    sprite->anchor,
                                    origin->rotation,
                                    image->sprite.renderable.tint
                                );
                            }
                        }
                    }

                    break;
                }

                case RenderEntry::Type::Rect:
                {
                    // GET THE FRAME AND USE THAT BROCHACHO

                    auto* frame = registry.try_get<Frame>(entry.entity);
                    if (frame && frame->renderable.visible)
                    {
                        float x = frame->position.scale.x * scene->logicalResolution.x + frame->position.offset.x;
                        float y = frame->position.scale.y * scene->logicalResolution.y + frame->position.offset.y;

                        float sx = frame->size.scale.x * scene->logicalResolution.x + frame->size.offset.x;
                        float sy = frame->size.scale.y * scene->logicalResolution.y + frame->size.offset.y;

                        if (frame->useOrigin)
                        {
                            auto* origin = registry.try_get<Origin2D>(entry.entity);
                            if (origin)
                            {
                                x += origin->position.x;
                                y += origin->position.y;

                                sx *= origin->scale.x;
                                sy *= origin->scale.y;
                            }
                            // Add origin position to the frame position
                            // Multiply frame size by scale


                        }

                        DrawRectangle(x, y, sx, sy, frame->renderable.tint);
                    }

                    break;
                }

                case RenderEntry::Type::Text:
                {
                    auto* text = registry.try_get<TextLabel>(entry.entity);
                    if (text && text->renderable.visible)
                    {
                        // Check if there is a frame then if there is an origin2d component,
                        // If so use them both, if not origin2d, just use frame positioning and sizes,
                        // If no frame, use origin2d,
                        // If neither, render nothing
                        auto* frame = registry.try_get<Frame>(entry.entity);
                        if (frame)
                        {
                            float x = frame->position.scale.x * scene->logicalResolution.x + frame->position.offset.x;
                            float y = frame->position.scale.y * scene->logicalResolution.y + frame->position.offset.y;

                            if (frame->useOrigin)
                            {
                                auto* origin = registry.try_get<Origin2D>(entry.entity);
                                if (origin)
                                {
                                    x += origin->position.x;
                                    y += origin->position.y;
                                }
                                // Add origin position to the frame position
                                // Multiply frame size by scale


                            }

                            auto* padding = registry.try_get<Padding>(entry.entity);
                            if (padding)
                            {
                                x += padding->left;
                                y += padding->top;
                            }

                            DrawText(text->text.c_str(), x, y, text->textSize, text->renderable.tint);
                        }
                        else
                        {
                            auto* origin = registry.try_get<Origin2D>(entry.entity);
                            if (origin)
                            {
                                float x = origin->position.x;
                                float y = origin->position.y;

                                auto* padding = registry.try_get<Padding>(entry.entity);
                                if (padding)
                                {
                                    x += padding->left;
                                    y += padding->top;
                                }

                                DrawText(text->text.c_str(), x, y, text->textSize, text->renderable.tint);
                            }
                        }
                    }

                    break;
                }
            }
        }

        EndMode2D();
    }

    // Draw screen-space entities
    for (const auto& entry : screenSpace)
    {
        switch (entry.type)
        {
            case RenderEntry::Type::Sprite:
            {
                auto* image = registry.try_get<Image>(entry.entity);
                if (image && image->sprite.renderable.visible && image->sprite.texture)
                {
                    // Because it's an image, we now check for a frame and adjust the position and size accordingly
                    // If there is no frame, then use the Origin2D
                    // If there is no Origin2D, or any sufficient position and size data, don't render the image

                    auto* frame = registry.try_get<Frame>(entry.entity);
                    auto* origin = registry.try_get<Origin2D>(entry.entity);
                    if (frame)
                    {
                        float x = frame->position.scale.x * scene->logicalResolution.x + frame->position.offset.x;
                        float y = frame->position.scale.y * scene->logicalResolution.y + frame->position.offset.y;

                        float sx = frame->size.scale.x * scene->logicalResolution.x + frame->size.offset.x;
                        float sy = frame->size.scale.y * scene->logicalResolution.y + frame->size.offset.y;

                        if (frame->useOrigin && origin)
                        {
                            // Add origin position to the frame position
                            // Multiply frame size by scale

                            x += origin->position.x;
                            y += origin->position.y;

                            sx *= origin->scale.x;
                            sy *= origin->scale.y;
                        }

                        DrawTexturePro(
                            *image->sprite.texture,
                            {0.0f, 0.0f, (float)image->sprite.texture->width, (float)image->sprite.texture->height},
                            {x, y, sx, sy},
                            image->sprite.anchor,
                            frame->rotation,
                            image->sprite.renderable.tint
                        );
                    }
                    else if (origin)
                    {
                        // Just use origin
                        DrawTexturePro(
                            *image->sprite.texture,
                            {0.0f, 0.0f, (float)image->sprite.texture->width, (float)image->sprite.texture->height},
                            {origin->position.x, origin->position.y, origin->scale.x, origin->scale.y},
                            image->sprite.anchor,
                            origin->rotation,
                            image->sprite.renderable.tint
                        );
                    }

                    // Else do nothing
                }
                else
                {
                    auto* sprite = registry.try_get<Sprite>(entry.entity);
                    if (sprite && sprite->renderable.visible && sprite->texture)
                    {
                        // Because it's a sprite, we don't need to check for a frame, so just render normally

                        auto* origin = registry.try_get<Origin2D>(entry.entity);
                        if (origin)
                        {
                            DrawTexturePro(
                                *sprite->texture,
                                {0.0f, 0.0f, (float)image->sprite.texture->width, (float)image->sprite.texture->height},
                                {origin->position.x, origin->position.y, origin->scale.x, origin->scale.y},
                                sprite->anchor,
                                origin->rotation,
                                image->sprite.renderable.tint
                            );
                        }
                    }
                }

                break;
            }

            case RenderEntry::Type::Rect:
            {
                // GET THE FRAME AND USE THAT BROCHACHO

                auto* frame = registry.try_get<Frame>(entry.entity);
                if (frame && frame->renderable.visible)
                {
                    float x = frame->position.scale.x * scene->logicalResolution.x + frame->position.offset.x;
                    float y = frame->position.scale.y * scene->logicalResolution.y + frame->position.offset.y;

                    float sx = frame->size.scale.x * scene->logicalResolution.x + frame->size.offset.x;
                    float sy = frame->size.scale.y * scene->logicalResolution.y + frame->size.offset.y;

                    if (frame->useOrigin)
                    {
                        auto* origin = registry.try_get<Origin2D>(entry.entity);
                        if (origin)
                        {
                            x += origin->position.x;
                            y += origin->position.y;

                            sx *= origin->scale.x;
                            sy *= origin->scale.y;
                        }
                        // Add origin position to the frame position
                        // Multiply frame size by scale
                    }

                    DrawRectangle(x, y, sx, sy, frame->renderable.tint);
                }

                break;
            }

            case RenderEntry::Type::Text:
            {
                auto* text = registry.try_get<TextLabel>(entry.entity);
                if (text && text->renderable.visible)
                {
                    // Check if there is a frame then if there is an origin2d component,
                    // If so use them both, if not origin2d, just use frame positioning and sizes,
                    // If no frame, use origin2d,
                    // If neither, render nothing
                    auto* frame = registry.try_get<Frame>(entry.entity);
                    if (frame)
                    {
                        float x = frame->position.scale.x * scene->logicalResolution.x + frame->position.offset.x;
                        float y = frame->position.scale.y * scene->logicalResolution.y + frame->position.offset.y;

                        if (frame->useOrigin)
                        {
                            auto* origin = registry.try_get<Origin2D>(entry.entity);
                            if (origin)
                            {
                                x += origin->position.x;
                                y += origin->position.y;
                            }
                            // Add origin position to the frame position
                            // Multiply frame size by scale
                        }

                        auto* padding = registry.try_get<Padding>(entry.entity);
                        if (padding)
                        {
                            x += padding->left;
                            y += padding->top;
                        }

                        DrawText(text->text.c_str(), x, y, text->textSize, text->renderable.tint);
                    }
                    else
                    {
                        auto* origin = registry.try_get<Origin2D>(entry.entity);
                        if (origin)
                        {
                            float x = origin->position.x;
                            float y = origin->position.y;

                            auto* padding = registry.try_get<Padding>(entry.entity);
                            if (padding)
                            {
                                x += padding->left;
                                y += padding->top;
                            }

                            DrawText(text->text.c_str(), x, y, text->textSize, text->renderable.tint);
                        }
                    }
                }

                break;
            }
        }
    }
}*/

void RenderSystem::Update(float deltaTime, entt::registry &registry)
{
    if (needsResort)
        RebuildRenderList(registry);

    // Partition render entries
    std::vector<RenderEntry> worldSpace;
    std::vector<RenderEntry> screenSpace;
    worldSpace.reserve(renderList.size());
    screenSpace.reserve(renderList.size());

    for (const auto &entry : renderList)
        (entry.isScreenSpace ? screenSpace : worldSpace).push_back(entry);

    auto drawSprite = [&](entt::entity e, const Scene *scene)
    {
        auto *sprite = registry.try_get<Sprite>(e);
        if (!sprite || !sprite->renderable.visible || !sprite->texture)
            return;

        auto *origin = registry.try_get<Origin2D>(e);

        Vector2 pos{0.f, 0.f};
        Vector2 size{(float)sprite->texture->width, (float)sprite->texture->height};
        float rotation = 0.f;

        if (origin)
        {
            pos = origin->position;
            size.x *= origin->scale.x;
            size.y *= origin->scale.y;
            rotation = origin->rotation;
        }

        DrawTexturePro(
            *sprite->texture,
            {0.f, 0.f, (float)sprite->texture->width, (float)sprite->texture->height},
            {pos.x, pos.y, size.x, size.y},
            sprite->anchor,
            rotation,
            sprite->renderable.tint);
    };

    auto drawImage = [&](entt::entity e, const Scene *scene)
    {
        auto *image = registry.try_get<Image>(e);
        if (!image || !image->sprite.texture || !image->sprite.renderable.visible)
            return;

        auto *frame = registry.try_get<Frame>(e);
        auto *origin = registry.try_get<Origin2D>(e);

        Vector2 pos{0, 0}, size{(float)image->sprite.texture->width, (float)image->sprite.texture->height};
        float rotation = 0.f;

        if (frame)
        {
            pos.x = frame->origin.position.scale.x * scene->logicalResolution.x + frame->origin.position.offset.x;
            pos.y = frame->origin.position.scale.y * scene->logicalResolution.y + frame->origin.position.offset.y;
            size.x = frame->origin.size.scale.x * scene->logicalResolution.x + frame->origin.size.offset.x;
            size.y = frame->origin.size.scale.y * scene->logicalResolution.y + frame->origin.size.offset.y;
            rotation = frame->rotation;

            if (frame->useOrigin && origin)
            {
                pos.x += origin->position.x;
                pos.y += origin->position.y;
                size.x *= origin->scale.x;
                size.y *= origin->scale.y;
            }
        }
        else if (origin)
        {
            pos = origin->position;
            size.x *= origin->scale.x;
            size.y *= origin->scale.y;
            rotation = origin->rotation;
        }

        DrawTexturePro(
            *image->sprite.texture,
            {0.f, 0.f, (float)image->sprite.texture->width, (float)image->sprite.texture->height},
            {pos.x, pos.y, size.x, size.y},
            image->sprite.anchor,
            rotation,
            image->sprite.renderable.tint);
    };

    auto drawRect = [&](entt::entity e, const Scene *scene)
    {
        auto *frame = registry.try_get<Frame>(e);
        if (!frame || !frame->renderable.visible)
            return;

        Vector2 pos{
            frame->origin.position.scale.x * scene->logicalResolution.x + frame->origin.position.offset.x,
            frame->origin.position.scale.y * scene->logicalResolution.y + frame->origin.position.offset.y};
        Vector2 size{
            frame->origin.size.scale.x * scene->logicalResolution.x + frame->origin.size.offset.x,
            frame->origin.size.scale.y * scene->logicalResolution.y + frame->origin.size.offset.y};

        if (frame->useOrigin)
        {
            if (auto *origin = registry.try_get<Origin2D>(e))
            {
                pos.x += origin->position.x;
                pos.y += origin->position.y;
                size.x *= origin->scale.x;
                size.y *= origin->scale.y;
            }
        }

        DrawRectangle(pos.x, pos.y, size.x, size.y, frame->renderable.tint);
    };

    auto drawText = [&](entt::entity e, const Scene *scene, bool isUI)
    {
        auto *text = registry.try_get<TextLabel>(e);
        if (!text || !text->renderable.visible)
            return;

        float x = 0.f, y = 0.f;

        if (isUI)
        {
            // For UI elements, origin.position.offset contains the final screen position
            x = text->origin.position.offset.x;
            y = text->origin.position.offset.y;
        }
        else
        {
            // World-space: calculate from frame + origin if present
            auto *frame = registry.try_get<Frame>(e);
            auto *origin = registry.try_get<Origin2D>(e);

            if (frame)
            {
                x = frame->origin.position.scale.x * scene->logicalResolution.x + frame->origin.position.offset.x;
                y = frame->origin.position.scale.y * scene->logicalResolution.y + frame->origin.position.offset.y;

                if (frame->useOrigin && origin)
                {
                    x += origin->position.x;
                    y += origin->position.y;
                }

                // Apply padding
                x += text->padding.left;
                y += text->padding.top;
            }
            else if (origin)
            {
                x = origin->position.x + text->padding.left;
                y = origin->position.y + text->padding.top;
            }
        }

        // Handle horizontal alignment
        float textWidth = MeasureText(text->text.c_str(), text->textSize);
        switch (text->horizontal)
        {
            case HorizontalAlignment::LEFT: break;
            case HorizontalAlignment::MIDDLE: x += (text->bounds.x - textWidth - text->padding.right) * 0.5f; break;
            case HorizontalAlignment::RIGHT: x += text->bounds.x - textWidth - text->padding.right; break;
        }

        // Handle vertical alignment
        switch (text->vertical)
        {
            case VerticalAlignment::TOP: break;
            case VerticalAlignment::MIDDLE: y += (text->bounds.y - text->textSize - text->padding.bottom) * 0.5f; break;
            case VerticalAlignment::BOTTOM: y += text->bounds.y - text->textSize - text->padding.bottom; break;
        }

        DrawText(text->text.c_str(), x, y, text->textSize, text->renderable.tint);
    };

    auto drawEntries = [&](const std::vector<RenderEntry> &entries, bool worldSpace)
    {
        if (worldSpace)
        {
            for (auto [camEntity, cam] : registry.view<Cam2D>().each())
            {
                BeginMode2D(cam.camera);
                for (const auto &entry : entries)
                {
                    switch (entry.type)
                    {
                    case RenderEntry::Type::Sprite:
                        drawSprite(entry.entity, scene);
                        break;
                    case RenderEntry::Type::Rect:
                        drawRect(entry.entity, scene);
                        break;
                    case RenderEntry::Type::Text:
                        drawText(entry.entity, scene, !entry.isScreenSpace);
                        break;
                    case RenderEntry::Type::Image:
                        drawImage(entry.entity, scene);
                        break;
                    }
                }
                EndMode2D();
            }
        }
        else
        {
            for (const auto &entry : entries)
            {
                switch (entry.type)
                {
                case RenderEntry::Type::Sprite:
                    drawSprite(entry.entity, scene);
                    break;
                case RenderEntry::Type::Rect:
                    drawRect(entry.entity, scene);
                    break;
                case RenderEntry::Type::Text:
                    drawText(entry.entity, scene, !entry.isScreenSpace);
                    break;
                case RenderEntry::Type::Image:
                    drawImage(entry.entity, scene);
                    break;
                }
            }
        }
    };

    drawEntries(worldSpace, true);
    drawEntries(screenSpace, false);
}

void ScriptSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Script>();

    for (auto entity : view)
    {
        auto &script = view.get<Script>(entity);

        if (!script.active)
        {
            if (script.OnCreate)
                script.OnCreate(scene, entity);
            script.active = true;
        }

        if (script.OnUpdate)
            script.OnUpdate(scene, entity, deltaTime);
    }
}

void ScriptSystem::OnDestroy(entt::registry &registry)
{
    auto view = registry.view<Script>();
    for (auto entity : view)
    {
        auto &script = view.get<Script>(entity);
        if (script.OnDestroy)
            script.OnDestroy(scene, entity);
    }
}

void GUISystem::Update(float deltaTime, entt::registry &registry)
{
    Vector2 mouse = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);

auto uilistView = registry.view<UIList, Frame>();
for (auto [entity, list, frame] : uilistView.each())
{
    auto children = scene->entityToChildren[entity];
    float offsetY = 0.0f;

    for (auto e : children)
    {
        auto *label = registry.try_get<TextLabel>(e);
        if (!label) continue;

        // Base position from parent frame
        Vector2 parentPos{
            frame.origin.position.scale.x * scene->logicalResolution.x + frame.origin.position.offset.x,
            frame.origin.position.scale.y * scene->logicalResolution.y + frame.origin.position.offset.y
        };

        // Child offset relative to parent frame
        label->origin.position.x = parentPos.x + label->padding.left;
        label->origin.position.y = parentPos.y + offsetY + label->padding.top;

        // Update offsetY for next child
        float childHeight = 0.f;
        if (auto *childFrame = registry.try_get<Frame>(e))
        {
            childHeight = childFrame->origin.size.scale.y * scene->logicalResolution.y + childFrame->origin.size.offset.y;
        }
        if (childHeight <= 0)
            childHeight = label->textSize; // fallback

        offsetY += childHeight;
    }
}




    auto buttonView = registry.view<Button>();
    for (auto entity : buttonView)
    {
        auto &button = buttonView.get<Button>(entity);
        if (!button.interactable)
            continue;

        Rectangle rect = UIOriginToRect(button.bounds, scene->logicalResolution.x, scene->logicalResolution.y);

        bool hovered = CheckCollisionPointRec(mouse, rect);
        auto *frame = registry.try_get<Frame>(entity);
        if (frame)
            frame->renderable.tint = hovered ? button.active : button.inactive;

        if (hovered)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                button.leftClickEvent->Fire();

            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                button.rightClickEvent->Fire();

            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
                button.middleClickEvent->Fire();

            if (!button.mouseHovering)
                button.mouseEnterEvent->Fire();

            button.mouseHovering = true;
        }
        else
        {
            if (button.mouseHovering)
                button.mouseLeaveEvent->Fire();
            
            button.mouseHovering = false;
        }
    }
}