#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "raylib/raylib.h"
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

        Renderable(unsigned int zlayer, bool isScreenSpace, Color tint)
            : zlayer(zlayer), isScreenSpace(isScreenSpace), tint(tint) {}
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
        float top, right, bottom, left;

        Padding(float top, float right, float bottom, float left)
            : top(top), right(right), bottom(bottom), left(left) {}
    };

    // New version of RectVisualizer, the basis of all GUI components (needed for all of them)
    struct Frame
    {
        Renderable renderable;

        FrameVector size;
        FrameVector position;
        float rotation;

        Vector2 anchor;

        bool useOrigin = true;

        Frame(Renderable renderable, FrameVector position, FrameVector size, float rotation, Vector2 anchor)
            : renderable(renderable), size(size), position(position), anchor(anchor) {}
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

        bool useOrigin = true;

        Image(Sprite sprite)
            : sprite(sprite) {}
    };

    struct [[deprecated("Use Frame instead")]] RectVisualizer : Renderable
    {
        Vector2 offset;
        Vector2 size;
        Color tint;

        RectVisualizer(Vector2 offset, Vector2 size, Color tint, unsigned int zlayer, bool isScreenSpace)
            : Renderable(zlayer, isScreenSpace, tint), offset(offset), size(size), tint(tint) {}
    };

    struct TextLabel
    {
        Renderable renderable;

        std::string text;
        float textSize;

        HorizontalAlignment horizontal;
        VerticalAlignment vertical;

        Vector2 bounds;

        TextLabel(Renderable renderable, std::string text, float textSize, HorizontalAlignment horizontal, VerticalAlignment vertical, Vector2 bounds)
            : renderable(renderable), text(text), textSize(textSize), horizontal(horizontal), vertical(vertical), bounds(bounds) {}
    };

    struct EventListener
    {
        std::string event;
        std::function<void(const std::vector<std::any> &)> connectedFunc;

        template <typename... Args>
        void Connect(std::function<void(Args...)> func)
        {
            connectedFunc = [func](const std::vector<std::any> &args)
            {
                Call(func, args, std::index_sequence_for<Args...>{});
            };
        }

        template <typename... Args, size_t... I>
        static void Call(const std::function<void(Args...)> &func, const std::vector<std::any> &args, std::index_sequence<I...>)
        {
            func(std::any_cast<Args>(args[I])...);
        }

        template <typename... Args>
        void Fire(Args &&...args)
        {
            if (connectedFunc)
                connectedFunc({std::forward<Args>(args)...});
        }
    };

    // Incomplete
    struct Button
    {
        EventListener *event;
        Rectangle bounds;

        bool interactable = true;
        Color active, inactive;

        Button(EventListener *event, bool interactable, Color active, Color inactive)
            : event(event), interactable(interactable), active(active), inactive(inactive) {}
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

}

#endif