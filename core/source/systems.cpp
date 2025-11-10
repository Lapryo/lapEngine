#include "systems.hpp"

#include <iostream>

using namespace lapCore;

void PhysicsSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Transform2D>();
    for (auto entity : view)
    {
        auto &transform = view.get<Transform2D>(entity);

        transform.position.x += transform.velocity.x * deltaTime;
        transform.position.y += transform.velocity.y * deltaTime;
    }
}

void RenderSystem::Connect(entt::registry& registry)
{
    registry.on_update<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<TextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<RectVisualizer>().connect<&RenderSystem::OnRenderableUpdated>(*this);
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
        renderList.push_back({e, s.zlayer, s.isScreenSpace, RenderEntry::Type::Sprite});
    }

    auto textView = registry.view<TextLabel>();
    for (auto e : textView) {
        const auto& t = textView.get<TextLabel>(e);
        renderList.push_back({e, t.zlayer, t.isScreenSpace, RenderEntry::Type::Text});
    }

    auto rectView = registry.view<RectVisualizer>();
    for (auto e : rectView) {
        const auto& r = rectView.get<RectVisualizer>(e);
        renderList.push_back({e, r.zlayer, r.isScreenSpace, RenderEntry::Type::Rect});
    }

    std::sort(renderList.begin(), renderList.end(), [](const auto& a, const auto& b) {
        if (a.isScreenSpace != b.isScreenSpace)
            return !a.isScreenSpace; // world first
        return a.zlayer < b.zlayer;
    });

    needsResort = false;
}

void RenderWorldSpace(entt::registry &registry)
{
    for (auto [camEntity, cam] : registry.view<Cam2D>().each())
    {
        BeginMode2D(cam.camera);

        // Draw world-space sprites
        for (auto [entity, transform, sprite] : registry.view<Transform2D, Sprite>().each())
            if (!sprite.isScreenSpace && sprite.texture && sprite.visible)
                DrawTexture(*sprite.texture, transform.position.x, transform.position.y, sprite.tint);

        // Draw world-space rects
        for (auto [entity, rect] : registry.view<RectVisualizer>().each())
            if (!rect.isScreenSpace && rect.visible) {
                const auto* transform = registry.try_get<Transform2D>(entity);
                float posX = rect.offset.x + (transform ? transform->position.x : 0);
                float posY = rect.offset.y + (transform ? transform->position.y : 0);
                DrawRectangle(posX, posY, rect.size.x, rect.size.y, rect.tint);
            }

        // Draw world-space text
        for (auto [entity, textLabel] : registry.view<TextLabel>().each())
        {
            if (!textLabel.isScreenSpace && textLabel.visible)
            {
                const auto* transform = registry.try_get<Transform2D>(entity);
                float posX = textLabel.offset.x + (transform ? transform->position.x : 0);
                float posY = textLabel.offset.y + (transform ? transform->position.y : 0);
                DrawText(textLabel.text.c_str(), posX, posY, textLabel.size, textLabel.color);
            }
        }

        EndMode2D();
    }
}

void RenderScreenSpace(entt::registry &registry)
{
    for (auto [entity, transform, sprite] : registry.view<Transform2D, Sprite>().each())
        if (sprite.isScreenSpace && sprite.texture && sprite.visible)
            DrawTexture(*sprite.texture, transform.position.x, transform.position.y, sprite.tint);

    for (auto [entity, rect] : registry.view<RectVisualizer>().each())
        if (rect.isScreenSpace && rect.visible) {
            const auto* transform = registry.try_get<Transform2D>(entity);
            float posX = rect.offset.x + (transform ? transform->position.x : 0);
            float posY = rect.offset.y + (transform ? transform->position.y : 0);
            DrawRectangle(posX, posY, rect.size.x, rect.size.y, rect.tint);
        }

    for (auto [entity, textLabel] : registry.view<TextLabel>().each())
        if (textLabel.isScreenSpace && textLabel.visible)
        {
            const auto* transform = registry.try_get<Transform2D>(entity);
            float posX = textLabel.offset.x + (transform ? transform->position.x : 0);
            float posY = textLabel.offset.y + (transform ? transform->position.y : 0);
            DrawText(textLabel.text.c_str(), posX, posY, textLabel.size, textLabel.color);
        }
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
                    auto& sprite = registry.get<Sprite>(entry.entity);
                    if (!sprite.visible || !sprite.texture) break;

                    auto* transform = registry.try_get<Transform2D>(entry.entity);
                    float x = transform ? transform->position.x : 0.f;
                    float y = transform ? transform->position.y : 0.f;

                    DrawTexture(*sprite.texture, x, y, sprite.tint);
                    break;
                }

                case RenderEntry::Type::Rect:
                {
                    auto& rect = registry.get<RectVisualizer>(entry.entity);
                    if (!rect.visible) break;

                    auto* transform = registry.try_get<Transform2D>(entry.entity);
                    float x = rect.offset.x + (transform ? transform->position.x : 0.f);
                    float y = rect.offset.y + (transform ? transform->position.y : 0.f);

                    DrawRectangle(x, y, rect.size.x, rect.size.y, rect.tint);
                    break;
                }

                case RenderEntry::Type::Text:
                {
                    auto& text = registry.get<TextLabel>(entry.entity);
                    if (!text.visible) break;

                    auto* transform = registry.try_get<Transform2D>(entry.entity);
                    float x = text.offset.x + (transform ? transform->position.x : 0.f);
                    float y = text.offset.y + (transform ? transform->position.y : 0.f);

                    DrawText(text.text.c_str(), x, y, text.size, text.color);
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
                auto& sprite = registry.get<Sprite>(entry.entity);
                if (!sprite.visible || !sprite.texture) break;

                auto* transform = registry.try_get<Transform2D>(entry.entity);
                float x = transform ? transform->position.x : 0.f;
                float y = transform ? transform->position.y : 0.f;

                DrawTexture(*sprite.texture, x, y, sprite.tint);
                break;
            }

            case RenderEntry::Type::Rect:
            {
                auto& rect = registry.get<RectVisualizer>(entry.entity);
                if (!rect.visible) break;

                auto* transform = registry.try_get<Transform2D>(entry.entity);
                float x = rect.offset.x + (transform ? transform->position.x : 0.f);
                float y = rect.offset.y + (transform ? transform->position.y : 0.f);

                DrawRectangle(x, y, rect.size.x, rect.size.y, rect.tint);
                break;
            }

            case RenderEntry::Type::Text:
            {
                auto& text = registry.get<TextLabel>(entry.entity);
                if (!text.visible) break;

                auto* transform = registry.try_get<Transform2D>(entry.entity);
                float x = text.offset.x + (transform ? transform->position.x : 0.f);
                float y = text.offset.y + (transform ? transform->position.y : 0.f);

                DrawText(text.text.c_str(), x, y, text.size, text.color);
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