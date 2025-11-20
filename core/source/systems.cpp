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
    // Rebuild when a property changes
    registry.on_update<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<TextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<Image>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<Frame>().connect<&RenderSystem::OnRenderableUpdated>(*this);

    // Rebuild when one is added
    registry.on_construct<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_construct<TextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_construct<Image>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_construct<Frame>().connect<&RenderSystem::OnRenderableUpdated>(*this);
}

void RenderSystem::OnRenderableUpdated(entt::registry &registry, Object object)
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
        renderList.push_back({e, t.frame.renderable.zlayer, t.frame.renderable.isScreenSpace, RenderEntry::Type::Text});
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

    auto drawSprite = [&](Object obj, const Scene *scene)
    {
        auto *sprite = registry.try_get<Sprite>(obj);

        const Texture2D* texture;

        auto it = scene->resources.textures.find(sprite->textureName);
        if (it != scene->resources.textures.end())
        {
            texture = &it->second;
        }
        else
            return;

        if (!sprite || !sprite->renderable.visible)
            return;

        auto *origin = registry.try_get<Origin2D>(obj);

        Vector2 pos{0.f, 0.f};
        Vector2 size{(float)texture->width, (float)texture->height};
        float rotation = 0.f;

        auto *rotData = registry.try_get<RotationalData>(obj);
        if (rotData)
        {
            rotation = rotData->rotation;
        }

        if (origin)
        {
            pos = origin->position;
            size.x *= origin->scale.x;
            size.y *= origin->scale.y;
        }

        DrawTexturePro(
            *texture,
            {0.f, 0.f, (float)texture->width, (float)texture->height},
            {pos.x, pos.y, size.x, size.y},
            (rotData ? rotData->anchor : (Vector2){0.0f, 0.0f}),
            rotation,
            sprite->renderable.tint);
    };

    auto drawImage = [&](Object obj, const Scene *scene)
    {
        auto *image = registry.try_get<Image>(obj);

        const Texture2D* texture;

        auto it = scene->resources.textures.find(image->sprite.textureName);
        if (it != scene->resources.textures.end())
        {
            texture = &it->second;
        }
        else
            return;

        if (!image || !image->sprite.renderable.visible)
            return;

        auto *frame = registry.try_get<Frame>(obj);
        auto *origin = registry.try_get<Origin2D>(obj);
        auto *rotData = registry.try_get<RotationalData>(obj);

        Vector2 pos{0, 0}, size{(float)texture->width, (float)texture->height};
        float rotation = 0.f;

        if (rotData)
        {
            rotation = rotData->rotation;
        }

        if (frame)
        {
            pos.x += frame->origin.position.scale.x * scene->logicalResolution.x + frame->origin.position.offset.x;
            pos.y += frame->origin.position.scale.y * scene->logicalResolution.y + frame->origin.position.offset.y;
            size.x += frame->origin.size.scale.x * scene->logicalResolution.x + frame->origin.size.offset.x;
            size.y += frame->origin.size.scale.y * scene->logicalResolution.y + frame->origin.size.offset.y;
        }

        if (origin)
        {
            pos.x += origin->position.x;
            pos.y += origin->position.y;

            size.x *= origin->scale.x;
            size.y *= origin->scale.y;
        }

        DrawTexturePro(
            *texture,
            {0.f, 0.f, (float)texture->width, (float)texture->height},
            {pos.x, pos.y, size.x, size.y},
            (rotData ? rotData->anchor : (Vector2){0.0f, 0.0f}),
            rotation,
            image->sprite.renderable.tint);
    };

    auto drawRect = [&](Object obj, const Scene *scene)
    {
        auto *frame = registry.try_get<Frame>(obj);
        if (!frame || !frame->renderable.visible)
            return;

        Rectangle rect = UIOriginToRect(frame->origin, scene->logicalResolution.x, scene->logicalResolution.y);

        if (auto *origin = registry.try_get<Origin2D>(obj))
        {
            rect.x += origin->position.x;
            rect.y += origin->position.y;
            rect.width *= origin->scale.x;
            rect.height *= origin->scale.y;
        }

        DrawRectangle(rect.x, rect.y, rect.width, rect.height, frame->renderable.tint);
    };

    auto drawText = [&](Object obj, const Scene *scene)
    {
        auto *text = registry.try_get<TextLabel>(obj);
        if (!text || !text->frame.renderable.visible)
            return;

        float x = 0.f, y = 0.f;

        x = text->frame.origin.position.scale.x * scene->logicalResolution.x + text->frame.origin.position.offset.x + text->textPadding.left;
        y = text->frame.origin.position.scale.y * scene->logicalResolution.y + text->frame.origin.position.offset.y + text->textPadding.top;

        auto *frame = registry.try_get<Frame>(obj);
        auto *origin = registry.try_get<Origin2D>(obj);

        if (frame)
        {
            x += frame->origin.position.scale.x * scene->logicalResolution.x + frame->origin.position.offset.x;
            y += frame->origin.position.scale.y * scene->logicalResolution.y + frame->origin.position.offset.y;
        }

        if (origin)
        {
            x += origin->position.x;
            y += origin->position.y;
        }

        // Handle horizontal alignment
        float textWidth = MeasureText(text->text.c_str(), text->textSize);
        switch (text->textAlignment.horizontal)
        {
            case HorizontalAlignment::LEFT: break;
            case HorizontalAlignment::MIDDLE: x += (text->textBounds.offset.x - textWidth - text->textPadding.right) * 0.5f; break;
            case HorizontalAlignment::RIGHT: x += text->textBounds.offset.x - textWidth - text->textPadding.right; break;
        }

        // Handle vertical alignment
        switch (text->textAlignment.vertical)
        {
            case VerticalAlignment::TOP: break;
            case VerticalAlignment::MIDDLE: y += (text->textBounds.offset.y - text->textSize - text->textPadding.bottom) * 0.5f; break;
            case VerticalAlignment::BOTTOM: y += text->textBounds.offset.y - text->textSize - text->textPadding.bottom; break;
        }

        DrawText(text->text.c_str(), x, y, text->textSize, text->frame.renderable.tint);
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
                        drawText(entry.entity, scene);
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
                    drawText(entry.entity, scene);
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
        if (!script.active) continue;

        if (!script.initiated)
        {
            auto createFunc = ScriptRegistry::onCreateFunctions[script.onCreateFunction];

            if (createFunc)
                createFunc(scene, entity);
            script.initiated = true;
        }

        auto updateFunc = ScriptRegistry::onUpdateFunctions[script.onUpdateFunction];
        if (updateFunc)
            updateFunc(scene, entity, deltaTime);
    }
}

void ScriptSystem::OnDestroy(entt::registry &registry)
{
    auto view = registry.view<Script>();
    for (auto entity : view)
    {
        auto &script = view.get<Script>(entity);
        if (!script.active) continue;

        auto destroyFunc = ScriptRegistry::onDestroyFunctions[script.onDestroyFunction];
        if (destroyFunc)
            destroyFunc(scene, entity);
    }
}

void GUISystem::Update(float deltaTime, entt::registry &registry)
{
    Vector2 mouse = GetMouseInViewportSpace(scene->logicalResolution.x, scene->logicalResolution.y);

    // This needs to be adjusted for an index value
    auto uilistView = registry.view<UIList, Frame>();
    for (auto [entity, list, frame] : uilistView.each())
    {
        auto children = scene->GetChildren(entity);
        float offsetY = 0.0f;

        for (auto e : children)
        {
            auto *label = registry.try_get<TextLabel>(e);

            if (!label) continue;

            label->frame.renderable.visible = frame.renderable.visible;

            Vector2 parentPos{
                frame.origin.position.scale.x * scene->logicalResolution.x + frame.origin.position.offset.x,
                frame.origin.position.scale.y * scene->logicalResolution.y + frame.origin.position.offset.y
            };

            label->frame.origin.position = frame.origin.position;

            auto *button = registry.try_get<UIButton>(e);
            if (button)
            {
                button->bounds.position = label->frame.origin.position;
            }

            // Child offset relative to parent frame
            //label->frame.origin.position.offset.x = parentPos.x + label->textPadding.left;
            //label->frame.origin.position.offset.y = parentPos.y + offsetY + label->textPadding.top;

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




    auto buttonView = registry.view<UIButton>();
    for (auto entity : buttonView)
    {
        auto &button = buttonView.get<UIButton>(entity);
        if (!button.active)
            continue;

        Rectangle rect = UIOriginToRect(button.bounds, scene->logicalResolution.x, scene->logicalResolution.y);
        bool hovered = CheckCollisionPointRec(mouse, rect);

        if (hovered)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                EventRegistry::Fire<>(button.events.events["left-click"]);
            }

            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                EventRegistry::Fire<>(button.events.events["right-click"]);

            if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
                EventRegistry::Fire<>(button.events.events["middle-click"]);

            if (!button.mouseHovering)
                EventRegistry::Fire<>(button.events.events["mouse-enter"]);

            button.mouseHovering = true;

            EventRegistry::Fire<>(button.events.events["mouse-hover"]);
        }
        else
        {
            if (button.mouseHovering)
                EventRegistry::Fire<>(button.events.events["mouse-leave"]);
            
            button.mouseHovering = false;
        }
    }
}