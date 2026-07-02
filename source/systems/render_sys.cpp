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
    renderList.clear();

    auto spriteView = registry.view<Sprite>();
    for (auto e : spriteView)
    {
        const auto &s = spriteView.get<Sprite>(e);
        renderList.push_back({e, s.renderable.zlayer, s.renderable.ySort, s.renderable.isScreenSpace, RenderType::Sprite});
    }

    auto imageView = registry.view<UIImage>();
    for (auto e : imageView)
    {
        const auto &s = imageView.get<UIImage>(e);
        renderList.push_back({e, s.sprite.renderable.zlayer, s.sprite.renderable.ySort, s.sprite.renderable.isScreenSpace, RenderType::Image});
    }

    auto textView = registry.view<UITextLabel>();
    for (auto e : textView)
    {
        const auto &t = textView.get<UITextLabel>(e);
        renderList.push_back({e, t.frame.renderable.zlayer, t.frame.renderable.ySort, t.frame.renderable.isScreenSpace, RenderType::Text});
    }

    auto frameView = registry.view<UIFrame>();
    for (auto e : frameView)
    {
        const auto &t = frameView.get<UIFrame>(e);
        renderList.push_back({e, t.renderable.zlayer, t.renderable.ySort, t.renderable.isScreenSpace, RenderType::Rect});
    }

    std::sort(renderList.begin(), renderList.end(), [](const auto &a, const auto &b)
    {
        if (a.isScreenSpace != b.isScreenSpace)
            return !a.isScreenSpace; // world first
        if (a.zlayer != b.zlayer)
            return a.zlayer < b.zlayer; 
        return a.ySort < b.ySort;
    });

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

        Rectangle rect = sprite->destRect;

        float rotation = 0.f;

        auto *origin = registry.try_get<Transform2D>(obj);
        if (origin)
        {
            rect.x += origin->position.x;
            rect.y += origin->position.y;
            rect.width *= origin->scale.x;
            rect.height *= origin->scale.y;

            rotation = origin->rotation;
        }

        sprite->renderable.drawRect = rect;

        if (texture)
            DrawTexturePro(
                *texture,
                sprite->sourceRect,
                rect,
                {rect.width / 2.f, rect.height / 2.f},
                rotation,
                sprite->renderable.tint);

        if (auto physics = registry.try_get<Physics2D>(obj)) {
            DrawRectangleLines(
                rect.x - (rect.width / 2.f), rect.y - (rect.height / 2.f), rect.width, rect.height, RED
            );
        }
    };

    auto drawImage = [&](Object obj, const Scene *scene)
    {
        auto *image = registry.try_get<lapCore::UIImage>(obj);
        if (!image || !image->sprite.renderable.visible)
            return;

        const Texture2D *texture = scene->world->resources.textures.TryGet(image->sprite.textureID);

        auto logicalRes = scene->world->window.logical_resolution;
        Rectangle rect = UIOriginToRect(image->origin, logicalRes);
        image->sprite.renderable.drawRect = rect;

        if (image->sprite.renderable.inUIList)
        {
            rect.x = image->origin.gui.position.x;
            rect.y = image->origin.gui.position.y;
            rect.height = image->origin.gui.size.y;
            rect.width = image->origin.gui.size.x;
        }

        auto anchorVec = FrameVectorToVec2(image->origin.transform.anchor, {rect.width, rect.height});

        if (texture)
            DrawTexturePro(
                *texture,
                image->sprite.sourceRect,
                rect,
                anchorVec,
                image->origin.transform.rotation,
                image->sprite.renderable.tint);
    };

    auto drawRect = [&](Object obj, const Scene *scene)
    {
        auto *frame = registry.try_get<UIFrame>(obj);
        if (!frame || !frame->renderable.visible)
            return;

        auto logicalRes = scene->world->window.logical_resolution;

        Rectangle rect = UIOriginToRect(frame->origin, logicalRes);
        frame->renderable.drawRect = rect;

        if (frame->renderable.inUIList)
        {
            rect.x = frame->origin.gui.position.x;
            rect.y = frame->origin.gui.position.y;
            rect.height = frame->origin.gui.size.y;
            rect.width = frame->origin.gui.size.x;
        }

        float rot = 0.f;

        if (auto *origin = registry.try_get<Transform2D>(obj))
        {
            rect.x += origin->position.x;
            rect.y += origin->position.y;
            rect.width *= origin->scale.x;
            rect.height *= origin->scale.y;
            rot = origin->rotation;
        }

        auto anchorVec = FrameVectorToVec2(frame->origin.transform.anchor, {rect.width, rect.height});

        DrawRectanglePro(rect, anchorVec, rot, frame->renderable.tint);
    };

    auto drawText = [&](Object obj, const Scene *scene)
    {
        auto *text = registry.try_get<UITextLabel>(obj);
        if (!text || !text->frame.renderable.visible)
            return;

        Vector2 logicalRes = scene->world->window.logical_resolution;

        Rectangle rect = UIOriginToRect(text->frame.origin, logicalRes);
        Vector2 textSizing = MeasureTextEx(GetFontDefault(), text->text.c_str(), text->fontSize, text->spacing);
        
        text->frame.renderable.drawRect = rect;
        text->textDrawRect = {rect.x, rect.y, textSizing.x, textSizing.y};

        if (text->frame.renderable.inUIList)
        {
            rect.x = text->frame.origin.gui.position.x;
            rect.y = text->frame.origin.gui.position.y;
            rect.height = text->frame.origin.gui.size.y;
            rect.width = text->frame.origin.gui.size.x;
        }

        Vector2 anchorVec = FrameVectorToVec2(text->frame.origin.transform.anchor, {rect.width, rect.height});

        rect.x += text->padding.left;
        rect.y += text->padding.top;

        switch (text->alignment.horizontal)
        {
            case HorizontalAlignment::MIDDLE:
            {
                rect.x += (rect.width - textSizing.x - text->padding.right) * 0.5f;
                break;
            }
            case HorizontalAlignment::RIGHT:
            {
                rect.x += rect.height - textSizing.x - text->padding.right;
                break;
            }
            default:
                break;
        }

        // Handle vertical alignment
        switch (text->alignment.vertical)
        {
            case VerticalAlignment::MIDDLE:
            {
                rect.y += (rect.height - textSizing.y - text->padding.bottom) * 0.5f;
                break;
            }
            case VerticalAlignment::BOTTOM:
            {
                rect.y += rect.height - textSizing.y - text->padding.bottom;
                break;
            }
            default:
                break;
        }

        DrawTextPro(
            GetFontDefault(), 
            text->text.c_str(), 
            {rect.x, rect.y}, 
            anchorVec, 
            text->frame.origin.transform.rotation, 
            text->fontSize, 
            text->spacing, 
            text->frame.renderable.tint
        );
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

    auto guiSys = scene->GetSystem<GUISystem>();
    if (guiSys) guiSys->ResetInUIList();
}
