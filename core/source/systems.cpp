#include "systems.hpp"
#include "scene.hpp"

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

void RenderSystem::Connect(entt::registry& registry)
{
    registry.on_update<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<TextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<Image>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<Frame>().connect<&RenderSystem::OnRenderableUpdated>(*this);
}

void RenderSystem::OnRenderableUpdated(entt::registry& registry, entt::entity entity) {
    needsResort = true;
}

void RenderSystem::RebuildRenderList(entt::registry& registry) {
    std::cout << "[RENDER] Rebuilding render list.\n";
    renderList.clear();

    auto spriteView = registry.view<Sprite>();
    for (auto e : spriteView) {
        const auto& s = spriteView.get<Sprite>(e);
        renderList.push_back({e, s.renderable.zlayer, s.renderable.isScreenSpace, RenderEntry::Type::Sprite});
    }

    auto imageView = registry.view<Image>();
    for (auto e : imageView)
    {
        const auto& s = imageView.get<Image>(e);
        renderList.push_back({e, s.sprite.renderable.zlayer, s.sprite.renderable.isScreenSpace, RenderEntry::Type::Sprite});
    }

    auto textView = registry.view<TextLabel>();
    for (auto e : textView) {
        const auto& t = textView.get<TextLabel>(e);
        renderList.push_back({e, t.renderable.zlayer, t.renderable.isScreenSpace, RenderEntry::Type::Text});
    }

    auto frameView = registry.view<Frame>();
    for (auto e : frameView) {
        const auto& t = frameView.get<Frame>(e);
        renderList.push_back({e, t.renderable.zlayer, t.renderable.isScreenSpace, RenderEntry::Type::Rect});
    }

    std::sort(renderList.begin(), renderList.end(), [](const auto& a, const auto& b) {
        if (a.isScreenSpace != b.isScreenSpace)
            return !a.isScreenSpace; // world first
        return a.zlayer < b.zlayer;
    });

    needsResort = false;
}

void RenderSystem::Update(float deltaTime, entt::registry& registry)
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
}

void ScriptSystem::Update(float deltaTime, entt::registry& registry)
{
    auto view = registry.view<Script>();

    for (auto entity : view) {
        auto &script = view.get<Script>(entity);

        if (!script.active) {
            if (script.OnCreate)
                script.OnCreate(scene, entity);
            script.active = true;
        }

        if (script.OnUpdate)
            script.OnUpdate(scene, entity, deltaTime);
    }
}

void ScriptSystem::OnDestroy(entt::registry& registry)
{
    auto view = registry.view<Script>();
    for (auto entity : view) {
        auto &script = view.get<Script>(entity);
        if (script.OnDestroy)
            script.OnDestroy(scene, entity);
    }
}

void GUISystem::Update(float deltaTime, entt::registry& registry)
{
    /*for (auto [entity, textLabel, textButton] : registry.view<TextLabel, TextButton>().each())
    {
        switch (textLabel.verticalAlignment)
        {
            case Alignment::LEFT:
            {

                break;
            }
            case Alignment::MIDDLE:
            {

                break;
            }
            case Alignment::RIGHT:
            {

                break;
            }
        }

        switch (textLabel.horizontalAlignment)
        {
            case Alignment::LEFT:
            {
                
                break;
            }
            case Alignment::MIDDLE:
            {

                break;
            }
            case Alignment::RIGHT:
            {

                break;
            }
        }

        switch (textButton.verticalAlignment)
        {
            case Alignment::LEFT:
            {
                
                break;
            }
            case Alignment::MIDDLE:
            {

                break;
            }
            case Alignment::RIGHT:
            {

                break;
            }
        }

        switch (textButton.horizontalAlignment)
        {
            case Alignment::LEFT:
            {
                
                break;
            }
            case Alignment::MIDDLE:
            {

                break;
            }
            case Alignment::RIGHT:
            {

                break;
            }
        }


    }*/
}