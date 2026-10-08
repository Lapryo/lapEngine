#include "core.hpp"

using namespace lapCore;
using namespace ELEMENTS::Render;
using namespace ELEMENTS::Render::UI;

// this was originally written by me, but got annoying real quick. just asked it to rewrite it and basically nothing is really different, just more comments and minor optimizations and clearer code. thanks mr. GPT

// ============================================================
// CONNECT
// ============================================================

void RenderSystem::Connect(entt::registry &registry)
{
    // Rebuild when a renderable property changes
    registry.on_update<Sprite>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_update<UITextLabel>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_update<UIImage>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_update<UIFrame>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);


    // Rebuild when a renderable is added
    registry.on_construct<Sprite>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_construct<UITextLabel>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_construct<UIImage>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);

    registry.on_construct<UIFrame>()
        .connect<&RenderSystem::OnRenderableUpdated>(*this);
}


// ============================================================
// RENDERABLE UPDATED
// ============================================================

void RenderSystem::OnRenderableUpdated(
    entt::registry &registry,
    Object object
)
{
    needsResort = true;
}


// ============================================================
// REBUILD RENDER LIST
// ============================================================

void RenderSystem::RebuildRenderList(
    entt::registry &registry
)
{
    renderList.clear();

    // --------------------------------------------------------
    // Sprites
    // --------------------------------------------------------

    auto spriteView =
        registry.view<Sprite>();

    for (auto e : spriteView)
    {
        const auto &s =
            spriteView.get<Sprite>(e);

        renderList.push_back({
            e,
            s.renderable.zlayer,
            s.renderable.ySort,
            s.renderable.isScreenSpace,
            RenderType::Sprite
        });
    }


    // --------------------------------------------------------
    // Images
    // --------------------------------------------------------

    auto imageView =
        registry.view<UIImage>();

    for (auto e : imageView)
    {
        const auto &image =
            imageView.get<UIImage>(e);

        renderList.push_back({
            e,
            image.sprite.renderable.zlayer,
            image.sprite.renderable.ySort,
            image.sprite.renderable.isScreenSpace,
            RenderType::Image
        });
    }


    // --------------------------------------------------------
    // Text
    // --------------------------------------------------------

    auto textView =
        registry.view<UITextLabel>();

    for (auto e : textView)
    {
        const auto &text =
            textView.get<UITextLabel>(e);

        renderList.push_back({
            e,
            text.frame.renderable.zlayer,
            text.frame.renderable.ySort,
            text.frame.renderable.isScreenSpace,
            RenderType::Text
        });
    }


    // --------------------------------------------------------
    // UI Frames
    // --------------------------------------------------------

    auto frameView =
        registry.view<UIFrame>();

    for (auto e : frameView)
    {
        const auto &frame =
            frameView.get<UIFrame>(e);

        renderList.push_back({
            e,
            frame.renderable.zlayer,
            frame.renderable.ySort,
            frame.renderable.isScreenSpace,
            RenderType::Rect
        });
    }


    // --------------------------------------------------------
    // Sort
    // --------------------------------------------------------

    std::sort(
        renderList.begin(),
        renderList.end(),
        [](const auto &a, const auto &b)
        {
            // World before screen-space
            if (a.isScreenSpace != b.isScreenSpace)
                return !a.isScreenSpace;

            // Lower z-layer first
            if (a.zlayer != b.zlayer)
                return a.zlayer < b.zlayer;

            // Lower y-sort first
            return a.ySort < b.ySort;
        }
    );

    needsResort = false;
}


// ============================================================
// UPDATE
// ============================================================

void RenderSystem::Update(
    float deltaTime,
    entt::registry &registry
)
{
    // --------------------------------------------------------
    // Rebuild render list if necessary.
    // --------------------------------------------------------

    if (needsResort)
        RebuildRenderList(registry);


    // --------------------------------------------------------
    // Partition render entries.
    // --------------------------------------------------------

    worldSpace.reserve(renderList.size());
    screenSpace.reserve(renderList.size());

    for (const auto &entry : renderList)
    {
        if (entry.isScreenSpace)
            screenSpace.push_back(entry);
        else
            worldSpace.push_back(entry);
    }


    // ========================================================
    // SPRITE
    // ========================================================

    auto drawSprite =
        [&](Object obj, const Scene *scene, Camera2D *camera)
    {
        auto *sprite =
            registry.try_get<Sprite>(obj);

        if (!sprite ||
            !sprite->renderable.visible)
        {
            return;
        }

        auto textureAsset = scene->world->resources.textures.GetAsset(sprite->textureID);
        if (!textureAsset) return;

        Rectangle rect =
            sprite->destRect;

        float rotation = 0.0f;


        // ----------------------------------------------------
        // World-space Transform2D
        // ----------------------------------------------------

        auto *origin =
            registry.try_get<Transform2D>(obj);

        if (origin)
        {
            rect.x += origin->position.x;
            rect.y += origin->position.y;

            rect.width *= origin->scale.x;
            rect.height *= origin->scale.y;

            rotation = origin->rotation;
        }

        sprite->renderable.drawRect =
            rect;


        // ----------------------------------------------------
        // Culling
        // ----------------------------------------------------

        if (!IsRectangleInViewportSpace(
                rect,
                scene->world->window.logical_resolution,
                camera) &&
            sprite->renderable.culling)
        {
            return;
        }


        // ----------------------------------------------------
        // Draw
        // ----------------------------------------------------

        if (textureAsset)
        {
            Rectangle source =
                sprite->sourceRect;

            constexpr float INSET = 0.01f;

            // Prevent sampling exactly on atlas boundaries.
            if (source.width > 0.0f)
            {
                source.x += INSET;
                source.width -= INSET * 2.0f;
            }
            else
            {
                source.x -= INSET;
                source.width += INSET * 2.0f;
            }

            if (source.height > 0.0f)
            {
                source.y += INSET;
                source.height -= INSET * 2.0f;
            }
            else
            {
                source.y -= INSET;
                source.height += INSET * 2.0f;
            }

            DrawTexturePro(
                *textureAsset,
                source,
                rect,
                {
                    rect.width / 2.0f,
                    rect.height / 2.0f
                },
                rotation,
                sprite->renderable.tint
            );
        }
    };


    // ========================================================
    // UI IMAGE
    // ========================================================

    auto drawImage =
        [&](Object obj, const Scene *scene, Camera2D *camera)
    {
        auto *image =
            registry.try_get<UIImage>(obj);

        if (!image ||
            !image->sprite.renderable.visible)
        {
            return;
        }

        const Texture2D *texture =
            &scene->world->resources.textures.TryGet(
                image->sprite.textureID
            )->asset;

        if (image->sprite.textureID == HASH_ID("apostrophe-s-ti"))
            std::cout << "apostrophe is being rendered.\n";

        // ----------------------------------------------------
        // Use the FINAL resolved absolute rectangle.
        //
        // This already includes:
        //
        // - hierarchy
        // - parent size
        // - anchor
        // - UIList overrides
        // ----------------------------------------------------

        Rectangle rect =
            image->transform.absoluteRect;

        image->sprite.renderable.drawRect =
            rect;


        // ----------------------------------------------------
        // Culling
        // ----------------------------------------------------

        if (!IsRectangleInViewportSpace(
                rect,
                scene->world->window.logical_resolution,
                camera) &&
            image->sprite.renderable.culling)
        {
            return;
        }

        // ----------------------------------------------------
        // Draw
        // ----------------------------------------------------

        if (texture)
        {
            DrawTexturePro(
                *texture,
                image->sprite.sourceRect,
                rect,
                {0.f, 0.f},
                image->transform.rotation,
                image->sprite.renderable.tint
            );
        }
    };


    // ========================================================
    // UI FRAME
    // ========================================================

    auto drawRect =
        [&](Object obj, const Scene *scene, Camera2D *camera)
    {
        auto *frame =
            registry.try_get<UIFrame>(obj);

        if (!frame ||
            !frame->renderable.visible)
        {
            return;
        }

        // ----------------------------------------------------
        // Use FINAL absolute rectangle.
        // ----------------------------------------------------

        Rectangle rect =
            frame->transform.absoluteRect;

        frame->renderable.drawRect =
            rect;


        // ----------------------------------------------------
        // Culling
        // ----------------------------------------------------

        if (!IsRectangleInViewportSpace(
                rect,
                scene->world->window.logical_resolution,
                camera) &&
            frame->renderable.culling)
        {
            return;
        }


        // ----------------------------------------------------
        // World Transform2D
        //
        // Keep this because UIFrame may still be usable as a
        // world-space renderable.
        // ----------------------------------------------------

        float rotation = 0.0f;

        auto *origin =
            registry.try_get<Transform2D>(obj);

        if (origin)
        {
            rect.x += origin->position.x;
            rect.y += origin->position.y;

            rect.width *= origin->scale.x;
            rect.height *= origin->scale.y;

            rotation = origin->rotation;
        }

        // ----------------------------------------------------
        // Draw
        // ----------------------------------------------------

        auto ui_gradient = registry.try_get<UIGradient>(obj);
        if (!ui_gradient)
        {
            DrawRectanglePro(
                rect,
                {0.0f, 0.0f},
                rotation,
                frame->renderable.tint
            );
        }
        else
        {
            int colorCount = std::min(
                static_cast<int>(ui_gradient->colorPoints.size()),
                16
            );

            if (colorCount == 0)
                return;

            float colorData[16 * 4]{};

            for (int i = 0; i < colorCount; i++)
            {
                colorData[i * 4 + 0] =
                    ui_gradient->colorPoints[i].r / 255.0f;

                colorData[i * 4 + 1] =
                    ui_gradient->colorPoints[i].g / 255.0f;

                colorData[i * 4 + 2] =
                    ui_gradient->colorPoints[i].b / 255.0f;

                colorData[i * 4 + 3] =
                    ui_gradient->colorPoints[i].a / 255.0f;
            }

            float angle =
                ui_gradient->angle * DEG2RAD;

            SetShaderValueV(
                UIGradient_fragShader,
                gradientColorLocation,
                colorData,
                SHADER_UNIFORM_VEC4,
                colorCount
            );

            SetShaderValue(
                UIGradient_fragShader,
                gradientColorCountLocation,
                &colorCount,
                SHADER_UNIFORM_INT
            );

            SetShaderValue(
                UIGradient_fragShader,
                gradientAngleLocation,
                &angle,
                SHADER_UNIFORM_FLOAT
            );

            Vector2 rectPosition = {
                rect.x,
                rect.y
            };

            Vector2 rectSize = {
                rect.width,
                rect.height
            };

            SetShaderValue(
                UIGradient_fragShader,
                gradientRectPositionLocation,
                &rectPosition,
                SHADER_UNIFORM_VEC2
            );

            SetShaderValue(
                UIGradient_fragShader,
                gradientRectSizeLocation,
                &rectSize,
                SHADER_UNIFORM_VEC2
            );

            BeginShaderMode(UIGradient_fragShader);

            DrawRectangleRec(
                rect,
                WHITE
            );

            EndShaderMode();
        }
    };


    // ========================================================
    // UI TEXT
    // ========================================================

    auto drawText =
        [&](Object obj, const Scene *scene, Camera2D *camera)
    {
        auto *text =
            registry.try_get<UITextLabel>(obj);

        if (!text ||
            !text->frame.renderable.visible)
        {
            return;
        }

        // ----------------------------------------------------
        // Final resolved UI rectangle.
        // ----------------------------------------------------

        Rectangle rect =
            text->frame.transform.absoluteRect;


        // ----------------------------------------------------
        // Font
        // ----------------------------------------------------

        auto find_font =
            scene->world->resources.fonts.TryGet(
                text->fontID
            );

        Font font =
            GetFontDefault();

        if (find_font)
        {
            font = find_font->asset;
        }


        // ----------------------------------------------------
        // Measure text
        // ----------------------------------------------------

        Vector2 textSizing =
            MeasureTextEx(
                font,
                text->text.c_str(),
                text->fontSize,
                text->spacing
            );


        // ----------------------------------------------------
        // Cache draw rectangles
        // ----------------------------------------------------

        text->frame.renderable.drawRect =
            rect;

        text->textDrawRect = {
            rect.x,
            rect.y,
            textSizing.x,
            textSizing.y
        };


        // ----------------------------------------------------
        // Culling
        // ----------------------------------------------------

        if (!IsRectangleInViewportSpace(
                rect,
                scene->world->window.logical_resolution,
                camera) &&
            text->frame.renderable.culling)
        {
            return;
        }

        // ----------------------------------------------------
        // Padding
        // ----------------------------------------------------

        rect.x += text->padding.left;
        rect.y += text->padding.top;


        // ----------------------------------------------------
        // Horizontal alignment
        // ----------------------------------------------------

        switch (text->alignment.horizontal)
        {
            case HorizontalAlignment::MIDDLE:
            {
                rect.x +=
                    (
                        rect.width -
                        textSizing.x -
                        text->padding.right
                    ) * 0.5f;

                break;
            }

            case HorizontalAlignment::RIGHT:
            {
                rect.x +=
                    rect.width -
                    textSizing.x -
                    text->padding.right;

                break;
            }

            default:
                break;
        }


        // ----------------------------------------------------
        // Vertical alignment
        // ----------------------------------------------------

        switch (text->alignment.vertical)
        {
            case VerticalAlignment::MIDDLE:
            {
                rect.y +=
                    (
                        rect.height -
                        textSizing.y -
                        text->padding.bottom
                    ) * 0.5f;

                break;
            }

            case VerticalAlignment::BOTTOM:
            {
                rect.y +=
                    rect.height -
                    textSizing.y -
                    text->padding.bottom;

                break;
            }

            default:
                break;
        }


        // ----------------------------------------------------
        // Draw
        // ----------------------------------------------------

        DrawTextPro(
            font,
            text->text.c_str(),
            {
                rect.x,
                rect.y
            },
            {0.f, 0.f},
            text->frame.transform.rotation,
            text->fontSize,
            text->spacing,
            text->frame.renderable.tint
        );
    };


    // ========================================================
    // DRAW ENTRIES
    // ========================================================

    auto drawEntries =
        [&](const std::vector<RenderEntry> &entries,
            bool worldSpace)
    {
        // ----------------------------------------------------
        // WORLD SPACE
        // ----------------------------------------------------

        if (worldSpace)
        {
            for (auto [camEntity, cam] :
                 registry.view<Camera2D>().each())
            {
                BeginMode2D(cam);

                for (const auto &entry : entries)
                {
                    switch (entry.type)
                    {
                        case RenderType::Sprite:
                            drawSprite(
                                entry.entity,
                                scene,
                                &cam
                            );
                            break;

                        case RenderType::Rect:
                            drawRect(
                                entry.entity,
                                scene,
                                &cam
                            );
                            break;

                        case RenderType::Text:
                            drawText(
                                entry.entity,
                                scene,
                                &cam
                            );
                            break;

                        case RenderType::Image:
                            drawImage(
                                entry.entity,
                                scene,
                                &cam
                            );
                            break;
                        default:
                            break;
                    }
                }

                EndMode2D();
            }
        }

        // ----------------------------------------------------
        // SCREEN SPACE
        // ----------------------------------------------------

        else
        {
            for (const auto &entry : entries)
            {
                switch (entry.type)
                {
                    case RenderType::Sprite:
                        drawSprite(
                            entry.entity,
                            scene,
                            nullptr
                        );
                        break;

                    case RenderType::Rect:
                        drawRect(
                            entry.entity,
                            scene,
                            nullptr
                        );
                        break;

                    case RenderType::Text:
                        drawText(
                            entry.entity,
                            scene,
                            nullptr
                        );
                        break;

                    case RenderType::Image:
                        drawImage(
                            entry.entity,
                            scene,
                            nullptr
                        );
                        break;
                    default:
                        break;
                }
            }
        }
    };


    // ========================================================
    // RENDER
    // ========================================================

    drawEntries(
        worldSpace,
        true
    );

    drawEntries(
        screenSpace,
        false
    );


    // ========================================================
    // RESET UI LIST STATE
    // ========================================================

    auto guiSys =
        scene->GetSystem<GUISystem>();

    if (guiSys)
        guiSys->ResetInUIList();


    // ========================================================
    // CLEAR PARTITIONED LISTS
    // ========================================================

    worldSpace.clear();
    screenSpace.clear();
}