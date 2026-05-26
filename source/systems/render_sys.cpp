#include "core.hpp"

using namespace lapCore;

void RenderSystem::Connect(entt::registry &registry)
{
    // Rebuild when a property changes
    registry.on_update<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<UITextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<UIImage>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<UIFrame>().connect<&RenderSystem::OnRenderableUpdated>(*this);

    // Rebuild when one is added
    registry.on_construct<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_construct<UITextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_construct<UIImage>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_construct<UIFrame>().connect<&RenderSystem::OnRenderableUpdated>(*this);
}

void RenderSystem::OnRenderableUpdated(entt::registry &registry, Object object)
{
    needsResort = true;
}

void RenderSystem::RebuildRenderList(entt::registry &registry)
{
    dbgln("[RENDER] Rebuilding render list.", LogType::INFO);
    renderList.clear();

    auto spriteView = registry.view<Sprite>();
    for (auto e : spriteView)
    {
        const auto &s = spriteView.get<Sprite>(e);
        renderList.push_back({e, s.renderable.zlayer, s.renderable.isScreenSpace, RenderType::Sprite});
    }

    auto imageView = registry.view<UIImage>();
    for (auto e : imageView)
    {
        const auto &s = imageView.get<UIImage>(e);
        renderList.push_back({e, s.sprite.renderable.zlayer, s.sprite.renderable.isScreenSpace, RenderType::Image});
    }

    auto textView = registry.view<UITextLabel>();
    for (auto e : textView)
    {
        const auto &t = textView.get<UITextLabel>(e);
        renderList.push_back({e, t.frame.renderable.zlayer, t.frame.renderable.isScreenSpace, RenderType::Text});
    }

    auto frameView = registry.view<UIFrame>();
    for (auto e : frameView)
    {
        const auto &t = frameView.get<UIFrame>(e);
        renderList.push_back({e, t.renderable.zlayer, t.renderable.isScreenSpace, RenderType::Rect});
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
        if (!sprite || !sprite->renderable.visible)
            return;

        const Texture2D* texture = scene->world->resources.textures.TryGet(sprite->textureID);

        Rectangle rect;
        rect.width = (float)texture->width;
        rect.height = (float)texture->height;

        float rotation = 0.f;

        auto *origin = registry.try_get<Transform2D>(obj);
        if (origin)
        {
            rect.x = origin->position.x;
            rect.y = origin->position.y;
            rect.width *= origin->scale.x;
            rect.height *= origin->scale.y;

            rotation = origin->rotation;
        }

        if (texture)
            DrawTexturePro(
                *texture,
                {0.f, 0.f, rect.width, rect.height},
                rect,
                {rect.width / 2.f, rect.height / 2.f},
                rotation,
                sprite->renderable.tint);
    };

    auto drawImage = [&](Object obj, const Scene *scene)
    {
        auto *image = registry.try_get<lapCore::UIImage>(obj);
        if (!image || !image->sprite.renderable.visible)
            return;

        const Texture2D *texture = scene->world->resources.textures.TryGet(image->sprite.textureID);

        Rectangle rect = UIOriginToRect(image->origin, scene->world->window.logical_resolution.x, scene->world->window.logical_resolution.y);

        if (texture)
            DrawTexturePro(
                *texture,
                {0.f, 0.f, (float)texture->width, (float)texture->height},
                rect,
                image->origin.anchor,
                image->origin.rotation,
                image->sprite.renderable.tint);
    };

    auto drawRect = [&](Object obj, const Scene *scene)
    {
        auto *frame = registry.try_get<UIFrame>(obj);
        if (!frame || !frame->renderable.visible)
            return;

        Rectangle rect = UIOriginToRect(frame->origin, scene->world->window.logical_resolution.x, scene->world->window.logical_resolution.y);
        float rot = 0.f;

        if (auto *origin = registry.try_get<Transform2D>(obj))
        {
            rect.x += origin->position.x;
            rect.y += origin->position.y;
            rect.width *= origin->scale.x;
            rect.height *= origin->scale.y;
            rot = origin->rotation;
        }

        DrawRectanglePro(rect, frame->origin.anchor, rot, frame->renderable.tint);
    };

    auto drawText = [&](Object obj, const Scene *scene)
    {
        auto *text = registry.try_get<UITextLabel>(obj);
        if (!text || !text->frame.renderable.visible)
            return;

        float x = 0.f, y = 0.f;

        x = text->frame.origin.position.scale.x * scene->world->window.logical_resolution.x + text->frame.origin.position.offset.x + text->padding.left;
        y = text->frame.origin.position.scale.y * scene->world->window.logical_resolution.y + text->frame.origin.position.offset.y + text->padding.top;

        // Handle horizontal alignment
        float textWidth = MeasureText(text->text.c_str(), text->fontSize);
        switch (text->alignment.horizontal)
        {
        case HorizontalAlignment::LEFT:
            break;
        case HorizontalAlignment::MIDDLE:
            x += (text->bounds.offset.x - textWidth - text->padding.right) * 0.5f;
            break;
        case HorizontalAlignment::RIGHT:
            x += text->bounds.offset.x - textWidth - text->padding.right;
            break;
        }

        // Handle vertical alignment
        switch (text->alignment.vertical)
        {
        case VerticalAlignment::TOP:
            break;
        case VerticalAlignment::MIDDLE:
            y += (text->bounds.offset.y - text->fontSize - text->padding.bottom) * 0.5f;
            break;
        case VerticalAlignment::BOTTOM:
            y += text->bounds.offset.y - text->fontSize - text->padding.bottom;
            break;
        }

        DrawText(text->text.c_str(), x, y, text->fontSize, text->frame.renderable.tint);
    };

    auto drawEntries = [&](const std::vector<RenderEntry> &entries, bool worldSpace)
    {
        if (worldSpace)
        {
            for (auto [camEntity, cam] : registry.view<Camera2D>().each())
            {
                BeginMode2D(cam);
                for (const auto &entry : entries)
                {
                    switch (entry.type)
                    {
                    case RenderType::Sprite:
                        drawSprite(entry.entity, scene);
                        break;
                    case RenderType::Rect:
                        drawRect(entry.entity, scene);
                        break;
                    case RenderType::Text:
                        drawText(entry.entity, scene);
                        break;
                    case RenderType::Image:
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
                case RenderType::Sprite:
                    drawSprite(entry.entity, scene);
                    break;
                case RenderType::Rect:
                    drawRect(entry.entity, scene);
                    break;
                case RenderType::Text:
                    drawText(entry.entity, scene);
                    break;
                case RenderType::Image:
                    drawImage(entry.entity, scene);
                    break;
                }
            }
        }
    };

    drawEntries(worldSpace, true);
    drawEntries(screenSpace, false);
}
