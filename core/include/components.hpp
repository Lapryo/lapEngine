#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "raylib/raylib.h"
#include "event.hpp"
#include <functional>
#include <any>

namespace lapCore
{
    class Scene; // <-- forward declaration
}

namespace lapCore
{
    struct Renderable
    {
        unsigned int zlayer;
        bool isScreenSpace;
        bool visible = true;
        Color tint;

        Renderable(unsigned int zlayer, bool isScreenSpace, bool visible, Color tint)
            : zlayer(zlayer), isScreenSpace(isScreenSpace), visible(visible), tint(tint) {}
    };

    enum class HorizontalAlignment
    {
        LEFT,
        MIDDLE,
        RIGHT
    };

    enum class VerticalAlignment
    {
        TOP,
        MIDDLE,
        BOTTOM
    };

    struct Origin2D
    {
        Vector2 position, scale;
        float rotation;
    };

    struct Physics2D
    {
        Vector2 velocity;
        Vector2 gravity;
    };

    struct FrameVector
    {
        Vector2 scale;
        Vector2 offset;
    };

    struct Padding
    {
        float top, bottom, left, right;
    };

    struct UIOrigin
    {
        FrameVector position;
        FrameVector size;

        UIOrigin(FrameVector position, FrameVector size)
            : position(position), size(size) {}
    };

    // New version of RectVisualizer, the basis of all GUI components (needed for all of them)
    struct Frame
    {
        Renderable renderable;

        UIOrigin origin;
        Vector2 anchor;
        float rotation;

        bool useOrigin = true;

        Frame(Renderable renderable, FrameVector position, FrameVector size, float rotation, Vector2 anchor)
            : renderable(renderable), origin(position, size), anchor(anchor) {}
    };

    struct UIList
    {
        FrameVector scrollSize;
        FrameVector displaySize;

        bool hScrollRight = true;
        bool vScrollBottom = true;

        bool maskOutsideContent = true;

        float scrollOffset = 0.f;
        float scrollSpeed = 20.f; // in pixels

        UIList(FrameVector scrollSize, FrameVector displaySize, bool hScrollRight, bool vScrollBottom, bool maskOutsideContent, float scrollOffset, float scrollSpeed)
            : scrollSize(scrollSize), displaySize(displaySize), hScrollRight(hScrollRight), vScrollBottom(vScrollBottom), maskOutsideContent(maskOutsideContent),
            scrollOffset(scrollOffset), scrollSpeed(scrollSpeed) {}
    };

    struct [[deprecated("Use Origin and Physics2D instead, will not work")]] Transform2D
    {
        Vector2 position, velocity, scale;
        float rotation;
    };

    struct Sprite
    {
        Renderable renderable;

        Texture2D *texture = nullptr;
        std::string textureName;

        Vector2 anchor;

        Sprite(Renderable renderable, Texture2D *texture, std::string textureName)
            : renderable(renderable), texture(texture), textureName(textureName) {}

        Sprite(Renderable renderable, std::string textureName)
            : renderable(renderable), textureName(textureName) {}
    };

    struct Image
    {
        Sprite sprite;

        UIOrigin origin;

        bool useOrigin = true;

        Image(Sprite sprite, FrameVector position, FrameVector size)
            : sprite(sprite), origin(position, size) {}
    };

    struct [[deprecated("Use Frame instead")]] RectVisualizer : Renderable
    {
        Vector2 offset;
        Vector2 size;
        Color tint;

        RectVisualizer(Vector2 offset, Vector2 size, Color tint, unsigned int zlayer, bool isScreenSpace)
            : Renderable(zlayer, isScreenSpace, true, tint), offset(offset), size(size), tint(tint) {}
    };

    struct TextLabel
    {
        Renderable renderable;

        UIOrigin origin;

        std::string text;
        float textSize;

        HorizontalAlignment horizontal;
        VerticalAlignment vertical;

        Vector2 bounds;
        Padding padding;

        Vector2 computedPosition;

        TextLabel(Renderable renderable, std::string text, float textSize, HorizontalAlignment horizontal, VerticalAlignment vertical, Vector2 bounds, Padding padding, FrameVector position, FrameVector size)
            : renderable(renderable), text(text), textSize(textSize), horizontal(horizontal), vertical(vertical), bounds(bounds), padding(padding), origin(position, size) {}
    };

    struct EventListener
    {
        std::string eventName;
        bool active = true;

        EventListener() = default;
        EventListener(const std::string& name) : eventName(name) {}

        template <typename... Args, typename Func>
        void Connect(Func&& func)
        {
            lapCore::EventRegistry::Connect<Args...>(eventName, std::forward<Func>(func));
        }

        template <typename... Args>
        void Fire(Args&&... args)
        {
            if (active)
                lapCore::EventRegistry::Fire<Args...>(eventName, std::forward<Args>(args)...);
        }
    };

    struct Button
    {
        EventListener* leftClickEvent;
        EventListener* rightClickEvent;
        EventListener* middleClickEvent;
        EventListener* mouseEnterEvent;
        EventListener* mouseLeaveEvent;
        EventListener* mouseHoverEvent;
        
        UIOrigin bounds;

        bool interactable = true;
        bool mouseHovering = false;
        Color active, inactive;

        Button(FrameVector position, FrameVector size, bool interactable, Color active, Color inactive)
            : bounds(position, size), interactable(interactable), active(active), inactive(inactive) {}
    };

    struct Cam2D
    {
        Camera2D camera;
        std::vector<entt::entity> exclude;
    };

    struct Script
    {
        std::function<void(lapCore::Scene *, entt::entity)> OnCreate;
        std::function<void(lapCore::Scene *, entt::entity, float)> OnUpdate;
        std::function<void(lapCore::Scene *, entt::entity)> OnDestroy;

        bool active = true;
        bool initiated = false;
    };

    template <typename T>
    struct Attribute
    {
        std::string name;
        T value;

        Attribute(std::string name, T value)
            : name(name), value(value) {}
    };

    struct BoolAttribute
    {
        std::string name;
        bool value;

        BoolAttribute(std::string name, bool value)
            : name(name), value(value) {}
    };

}

#endif