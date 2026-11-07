#include "systems/render_sys.hpp"
#include "world.hpp"
#include "elements.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace lapCore;
using namespace ELEMENTS::Render;
using namespace ELEMENTS::Render::UI;
using namespace ELEMENTS::Physics;

namespace
{
    // The visible (top-left based) rectangle and its local rotation pivot.
    struct DrawGeometry
    {
        Rectangle rect{};
        Vector2 anchor{};
        float rotation = 0.0f; // raylib degrees
    };

    Vector2 GetAnchor(const UITransform &transform, float width, float height)
    {
        return {
            width * transform.anchor.scale.x + transform.anchor.offset.x,
            height * transform.anchor.scale.y + transform.anchor.offset.y
        };
    }

    // Screen-space UI has already been positioned (including its anchor)
    // by GUISystem. World-space UI is authored in local coordinates and
    // must resolve its anchor here instead; its physics body supplies the
    // final world-space translation.
    DrawGeometry GetUIGeometry(
        Scene *scene,
        registry &registry,
        Object entity,
        const UITransform &transform,
        bool isScreenSpace)
    {
        DrawGeometry out{};
        out.rotation = transform.rotation;

        if (isScreenSpace)
        {
            out.rect = transform.absoluteRect;
        }
        else
        {
            Rectangle parent = transform.parentAbsRect;

            // A world object with no resolved UI parent uses the logical
            // canvas for its scale-based position/size components.
            if (parent.width == 0.0f && parent.height == 0.0f)
            {
                const Vector2 size = scene->world->window.logical_resolution;
                parent = {0.0f, 0.0f, size.x, size.y};
            }

            const float width =
                parent.width * transform.size.scale.x + transform.size.offset.x;
            const float height =
                parent.height * transform.size.scale.y + transform.size.offset.y;
            const Vector2 anchor = GetAnchor(transform, width, height);

            out.rect = {
                parent.x + parent.width * transform.position.scale.x +
                    transform.position.offset.x - anchor.x,
                parent.y + parent.height * transform.position.scale.y +
                    transform.position.offset.y - anchor.y,
                width,
                height
            };

            // Transform2D is physics-owned, not a second set of UI layout
            // parameters. Its position is the Box2D body's world position.
            if (const auto *physics = registry.try_get<Transform2D>(entity))
            {
                out.rect.x += physics->position.x;
                out.rect.y += physics->position.y;
                out.rotation += physics->rotation;
            }
        }

        out.anchor = GetAnchor(transform, out.rect.width, out.rect.height);
        return out;
    }

    // raylib's DrawRectanglePro / DrawTexturePro interprets destination.x/y
    // as the rotation pivot when origin != {0,0}. Geometry.rect, however,
    // is always the actual unrotated top-left rectangle.
    Rectangle PivotDestination(const DrawGeometry &geometry)
    {
        return {
            geometry.rect.x + geometry.anchor.x,
            geometry.rect.y + geometry.anchor.y,
            geometry.rect.width,
            geometry.rect.height
        };
    }

    // Returns a conservative axis-aligned bound in the same coordinate
    // system as geometry.rect; culling MUST happen after physics translation.
    Rectangle RotatedBounds(const DrawGeometry &geometry)
    {
        const Rectangle rect = geometry.rect;
        if (geometry.rotation == 0.0f)
            return rect;

        const float radians = geometry.rotation * DEG2RAD;
        const float cosine = cos(radians);
        const float sine = sin(radians);
        const Vector2 pivot = {
            rect.x + geometry.anchor.x,
            rect.y + geometry.anchor.y
        };

        const Vector2 corners[4] = {
            {-geometry.anchor.x, -geometry.anchor.y},
            {rect.width - geometry.anchor.x, -geometry.anchor.y},
            {rect.width - geometry.anchor.x, rect.height - geometry.anchor.y},
            {-geometry.anchor.x, rect.height - geometry.anchor.y}
        };

        float minX = numeric_limits<float>::infinity();
        float minY = numeric_limits<float>::infinity();
        float maxX = -numeric_limits<float>::infinity();
        float maxY = -numeric_limits<float>::infinity();

        for (const Vector2 &corner : corners)
        {
            const float x = pivot.x + corner.x * cosine - corner.y * sine;
            const float y = pivot.y + corner.x * sine + corner.y * cosine;
            minX = min(minX, x);
            minY = min(minY, y);
            maxX = max(maxX, x);
            maxY = max(maxY, y);
        }

        return {minX, minY, maxX - minX, maxY - minY};
    }

    // Original sprite convention: destRect.x/y is the drawing pivot,
    // not the visible rectangle's top-left corner. Preserve that behavior.
    // DrawTexturePro itself receives the pivot as destination.x/y.
    DrawGeometry GetSpriteGeometry(
        const Sprite &sprite,
        const Transform2D *physics,
        bool isScreenSpace)
    {
        Rectangle destination = sprite.destRect;
        float rotation = 0.0f;

        if (!isScreenSpace && physics)
        {
            destination.x += physics->position.x;
            destination.y += physics->position.y;
            destination.width *= physics->scale.x;
            destination.height *= physics->scale.y;
            rotation = physics->rotation;
        }

        DrawGeometry out{};
        out.anchor = {destination.width * 0.5f, destination.height * 0.5f};
        out.rect = {
            destination.x - out.anchor.x,
            destination.y - out.anchor.y,
            destination.width,
            destination.height
        };
        out.rotation = rotation;
        return out;
    }

    // Keep the original tiny atlas-edge inset for Sprite source rectangles.
    Rectangle InsetSpriteSource(Rectangle source)
    {
        constexpr float INSET = 0.01f;
        if (source.width > 0.0f)
        {
            source.x += INSET;
            source.width -= 2.0f * INSET;
        }
        else
        {
            source.x -= INSET;
            source.width += 2.0f * INSET;
        }

        if (source.height > 0.0f)
        {
            source.y += INSET;
            source.height -= 2.0f * INSET;
        }
        else
        {
            source.y -= INSET;
            source.height += 2.0f * INSET;
        }
        return source;
    }
}

void RenderSystem::Connect(registry &registry)
{
    registry.on_construct<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_destroy<Sprite>().connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_construct<UIImage>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<UIImage>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_destroy<UIImage>().connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_construct<UITextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<UITextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_destroy<UITextLabel>().connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_construct<UIFrame>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_update<UIFrame>().connect<&RenderSystem::OnRenderableUpdated>(*this);
    registry.on_destroy<UIFrame>().connect<&RenderSystem::OnRenderableUpdated>(*this);
}

void RenderSystem::OnRenderableUpdated(registry &registry, Object object)
{
    needsResort = true;
}

void RenderSystem::RebuildRenderList(registry &registry)
{
    renderList.clear();
    renderList.reserve(
        registry.storage<Sprite>().size() +
        registry.storage<UIImage>().size() +
        registry.storage<UITextLabel>().size() +
        registry.storage<UIFrame>().size());

    for (auto [entity, sprite] : registry.view<Sprite>().each())
        renderList.push_back({
            entity, sprite.renderable.zlayer, sprite.renderable.ySort,
            sprite.renderable.isScreenSpace, RenderType::Sprite
        });

    for (auto [entity, image] : registry.view<UIImage>().each())
        renderList.push_back({
            entity, image.sprite.renderable.zlayer, image.sprite.renderable.ySort,
            image.sprite.renderable.isScreenSpace, RenderType::Image
        });

    for (auto [entity, text] : registry.view<UITextLabel>().each())
        renderList.push_back({
            entity, text.frame.renderable.zlayer, text.frame.renderable.ySort,
            text.frame.renderable.isScreenSpace, RenderType::Text
        });

    for (auto [entity, frame] : registry.view<UIFrame>().each())
    {
        // Scene root is a layout canvas, never a visible overlay.
        if (entity == scene->root)
            continue;
        renderList.push_back({
            entity, frame.renderable.zlayer, frame.renderable.ySort,
            frame.renderable.isScreenSpace, RenderType::Rect
        });
    }

    sort(renderList.begin(), renderList.end(),
        [](const auto &a, const auto &b)
        {
            if (a.isScreenSpace != b.isScreenSpace)
                return !a.isScreenSpace;
            if (a.zlayer != b.zlayer)
                return a.zlayer < b.zlayer;
            return a.ySort < b.ySort;
        });

    needsResort = false;
}

void RenderSystem::Update(float deltaTime, registry &registry)
{
    if (needsResort)
        RebuildRenderList(registry);

    // These are retained member vectors. Clearing at the start also avoids
    // accumulating entries if a previous update returned prematurely.
    worldSpace.clear();
    screenSpace.clear();
    worldSpace.reserve(renderList.size());
    screenSpace.reserve(renderList.size());

    for (const auto &entry : renderList)
    {
        if (entry.isScreenSpace)
            screenSpace.push_back(entry);
        else
            worldSpace.push_back(entry);
    }

    const Vector2 resolution = scene->world->window.logical_resolution;

    // Keep viewport testing here: IsRectangleInViewportSpace may be a
    // RenderSystem member rather than a free function in some lapEngine builds.
    auto isVisible = [&](const DrawGeometry &geometry, Camera2D *camera)
    {
        return IsRectangleInViewportSpace(
            RotatedBounds(geometry), resolution, camera);
    };

    auto drawSprite = [&](Object obj, Camera2D *camera)
    {
        auto *sprite = registry.try_get<Sprite>(obj);
        if (!sprite || !isRenderableVisible(sprite->renderable))
            return;

        auto textureAsset = scene->world->resources.textures.GetAsset(sprite->textureID);
        if (!textureAsset)
            return;

        const DrawGeometry geometry = GetSpriteGeometry(
            *sprite, registry.try_get<Transform2D>(obj),
            sprite->renderable.isScreenSpace);

        sprite->renderable.drawRect = geometry.rect;
        if (sprite->renderable.culling && !isVisible(geometry, camera))
            return;

        DrawTexturePro(
            *textureAsset,
            InsetSpriteSource(sprite->sourceRect),
            PivotDestination(geometry),
            geometry.anchor,
            geometry.rotation,
            sprite->renderable.tint);
    };

    auto drawImage = [&](Object obj, Camera2D *camera)
    {
        auto *image = registry.try_get<UIImage>(obj);
        if (!image || !isRenderableVisible(image->sprite.renderable))
            return;

        auto *textureEntry = scene->world->resources.textures.TryGet(image->sprite.textureID);
        if (!textureEntry)
            return;

        const DrawGeometry geometry = GetUIGeometry(
            scene, registry, obj, image->transform,
            image->sprite.renderable.isScreenSpace);

        image->sprite.renderable.drawRect = geometry.rect;
        if (image->sprite.renderable.culling && !isVisible(geometry, camera))
            return;

        DrawTexturePro(
            textureEntry->asset,
            image->sprite.sourceRect,
            PivotDestination(geometry),
            geometry.anchor,
            geometry.rotation,
            image->sprite.renderable.tint);
    };

    auto drawRect = [&](Object obj, Camera2D *camera)
    {
        auto *frame = registry.try_get<UIFrame>(obj);
        if (!frame || !isRenderableVisible(frame->renderable))
            return;

        const DrawGeometry geometry = GetUIGeometry(
            scene, registry, obj, frame->transform,
            frame->renderable.isScreenSpace);

        frame->renderable.drawRect = geometry.rect;
        if (frame->renderable.culling && !isVisible(geometry, camera))
            return;

        const Rectangle destination = PivotDestination(geometry);
        auto *gradient = registry.try_get<UIGradient>(obj);

        if (!gradient)
        {
            DrawRectanglePro(
                destination, geometry.anchor, geometry.rotation,
                frame->renderable.tint);
        }
        else
        {
            const int colorCount = min(
                static_cast<int>(gradient->colorPoints.size()), 16);
            if (colorCount == 0)
                return;

            float colorData[16 * 4]{};
            for (int i = 0; i < colorCount; ++i)
            {
                colorData[i * 4 + 0] = gradient->colorPoints[i].r / 255.0f;
                colorData[i * 4 + 1] = gradient->colorPoints[i].g / 255.0f;
                colorData[i * 4 + 2] = gradient->colorPoints[i].b / 255.0f;
                colorData[i * 4 + 3] = gradient->colorPoints[i].a / 255.0f;
            }

            const float angle = gradient->angle * DEG2RAD;
            const Vector2 rectPosition = {geometry.rect.x, geometry.rect.y};
            const Vector2 rectSize = {geometry.rect.width, geometry.rect.height};

            SetShaderValueV(
                UIGradient_fragShader, gradientColorLocation,
                colorData, SHADER_UNIFORM_VEC4, colorCount);
            SetShaderValue(
                UIGradient_fragShader, gradientColorCountLocation,
                &colorCount, SHADER_UNIFORM_INT);
            SetShaderValue(
                UIGradient_fragShader, gradientAngleLocation,
                &angle, SHADER_UNIFORM_FLOAT);
            SetShaderValue(
                UIGradient_fragShader, gradientRectPositionLocation,
                &rectPosition, SHADER_UNIFORM_VEC2);
            SetShaderValue(
                UIGradient_fragShader, gradientRectSizeLocation,
                &rectSize, SHADER_UNIFORM_VEC2);

            BeginShaderMode(UIGradient_fragShader);
            DrawRectanglePro(destination, geometry.anchor, geometry.rotation, WHITE);
            EndShaderMode();
        }

        if (debugLevel != DebugLevel::NONE)
        {
            const Vector2 pivot = {destination.x, destination.y};
            DrawCircleV(pivot, 2.0f, PURPLE);
            DrawRectangleLinesEx(RotatedBounds(geometry), 1.0f, PURPLE);
        }
    };

    auto drawText = [&](Object obj, Camera2D *camera)
    {
        auto *label = registry.try_get<UITextLabel>(obj);
        if (!label || !isRenderableVisible(label->frame.renderable))
            return;

        const DrawGeometry geometry = GetUIGeometry(
            scene, registry, obj, label->frame.transform,
            label->frame.renderable.isScreenSpace);

        label->frame.renderable.drawRect = geometry.rect;
        if (label->frame.renderable.culling && !isVisible(geometry, camera))
            return;

        Font font = GetFontDefault();
        auto *foundFont = scene->world->resources.fonts.TryGet(label->fontID);
        if (foundFont)
            font = foundFont->asset;

        const Vector2 textSize = MeasureTextEx(
            font, label->text.c_str(), label->fontSize, label->spacing);

        Vector2 textPosition = {
            geometry.rect.x + label->padding.left,
            geometry.rect.y + label->padding.top
        };

        switch (label->alignment.horizontal)
        {
            case HorizontalAlignment::MIDDLE:
                textPosition.x += (geometry.rect.width - textSize.x - label->padding.right) * 0.5f;
                break;
            case HorizontalAlignment::RIGHT:
                textPosition.x += geometry.rect.width - textSize.x - label->padding.right;
                break;
            default:
                break;
        }

        switch (label->alignment.vertical)
        {
            case VerticalAlignment::MIDDLE:
                textPosition.y += (geometry.rect.height - textSize.y - label->padding.bottom) * 0.5f;
                break;
            case VerticalAlignment::BOTTOM:
                textPosition.y += geometry.rect.height - textSize.y - label->padding.bottom;
                break;
            default:
                break;
        }

        label->textDrawRect = {
            textPosition.x, textPosition.y,
            textSize.x, textSize.y
        };

        const Vector2 pivot = {
            geometry.rect.x + geometry.anchor.x,
            geometry.rect.y + geometry.anchor.y
        };
        const Vector2 textOrigin = {
            pivot.x - textPosition.x,
            pivot.y - textPosition.y
        };

        DrawTextPro(
            font, label->text.c_str(), pivot, textOrigin,
            geometry.rotation, label->fontSize, label->spacing,
            label->frame.renderable.tint);
    };

    auto drawEntries = [&](const vector<RenderEntry> &entries, Camera2D *camera)
    {
        for (const auto &entry : entries)
        {
            switch (entry.type)
            {
                case RenderType::Sprite:
                    drawSprite(entry.entity, camera);
                    break;
                case RenderType::Image:
                    drawImage(entry.entity, camera);
                    break;
                case RenderType::Rect:
                    drawRect(entry.entity, camera);
                    break;
                case RenderType::Text:
                    drawText(entry.entity, camera);
                    break;
                default:
                    break;
            }
        }
    };

    // World-space renderables are drawn once per camera.
    for (auto [entity, camera] : registry.view<Camera2D>().each())
    {
        BeginMode2D(camera);
        drawEntries(worldSpace, &camera);
        EndMode2D();
    }

    // Screen-space renderables are drawn outside any camera transform.
    drawEntries(screenSpace, nullptr);

    // GUISystem::Update() already resets/recomputes UIList state before
    // rendering. Do not reset that state again at the end of this update.
    worldSpace.clear();
    screenSpace.clear();
}
