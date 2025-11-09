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
            if (!sprite.isScreenSpace && sprite.texture)
                DrawTexture(*sprite.texture, transform.position.x, transform.position.y, sprite.tint);

        // Draw world-space rects
        for (auto [entity, transform, rect] : registry.view<Transform2D, RectVisualizer>().each())
            if (!rect.isScreenSpace)
                DrawRectangle(transform.position.x, transform.position.y, rect.size.x, rect.size.y, rect.tint);

        // Draw world-space text
        for (auto [entity, textLabel] : registry.view<TextLabel>().each())
        {
            if (!textLabel.isScreenSpace)
            {
                const auto* transform = registry.try_get<Transform2D>(entity);
                float posX = textLabel.textPosition.x + (transform ? transform->position.x : 0);
                float posY = textLabel.textPosition.y + (transform ? transform->position.y : 0);
                DrawText(textLabel.text.c_str(), posX, posY, textLabel.textSize, textLabel.textColor);
            }
        }

        EndMode2D();
    }
}

void RenderScreenSpace(entt::registry &registry)
{
    for (auto [entity, transform, sprite] : registry.view<Transform2D, Sprite>().each())
        if (sprite.isScreenSpace && sprite.texture)
            DrawTexture(*sprite.texture, transform.position.x, transform.position.y, sprite.tint);

    for (auto [entity, transform, rect] : registry.view<Transform2D, RectVisualizer>().each())
        if (rect.isScreenSpace)
            DrawRectangle(transform.position.x, transform.position.y, rect.size.x, rect.size.y, rect.tint);

    for (auto [entity, textLabel] : registry.view<TextLabel>().each())
        if (textLabel.isScreenSpace)
        {
            const auto* transform = registry.try_get<Transform2D>(entity);
            float posX = textLabel.textPosition.x + (transform ? transform->position.x : 0);
            float posY = textLabel.textPosition.y + (transform ? transform->position.y : 0);
            DrawText(textLabel.text.c_str(), posX, posY, textLabel.textSize, textLabel.textColor);
        }
}

void RenderSystem::Update(float deltaTime, entt::registry& registry)
{
    if (needsResort)
        RebuildRenderList(registry);
    
    RenderWorldSpace(registry);
    RenderScreenSpace(registry);
}