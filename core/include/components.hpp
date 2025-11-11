#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "raylib/raylib.h"
#include <functional>
#include <any>

namespace lapCore {
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

    enum class Alignment
    {
        LEFT,
        MIDDLE,
        RIGHT
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
        Alignment horizontal;
        Alignment vertical;

        Frame(Renderable renderable, FrameVector position, FrameVector size, float rotation, Alignment horizontal, Alignment vertical, Vector2 anchor)
            : renderable(renderable), horizontal(horizontal), vertical(vertical), anchor(anchor), size(size), position(position) {}
    };

    struct [[deprecated("Use Origin and Physics2D instead, will not work")]] Transform2D
    {
        Vector2 position, velocity, scale;
        float rotation;
    };

    struct Sprite
    {
        Renderable renderable;
        Texture2D* texture = nullptr;
        std::string textureName;

        Sprite(Renderable renderable, Texture2D* texture, const std::string &textureName)
            : renderable(renderable), texture(texture), textureName(textureName) {}

        Sprite(Renderable renderable, const std::string &textureName)
            : renderable(renderable), textureName(textureName) {}
    };

    struct Image
    {
        Sprite sprite;
        Frame frame;

        Image(Sprite sprite, Frame frame)
            : sprite(sprite), frame(frame) {}
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
        Frame frame;

        std::string text;
        float textSize;
        
        TextLabel(Renderable renderable, Frame frame, const std::string &text, float textSize)
            : renderable(renderable), frame(frame), text(text), textSize(textSize) {}
    };

    struct EventListener
    {
        std::string event;
        std::function<void(const std::vector<std::any>&)> connectedFunc;

        template <typename... Args>
        void Connect(std::function<void(Args...)> func)
        {
            connectedFunc = [func](const std::vector<std::any>& args)
            {
                Call(func, args, std::index_sequence_for<Args...>{});
            };
        }

        template <typename... Args, size_t... I>
        static void Call(const std::function<void(Args...)>& func, const std::vector<std::any>& args, std::index_sequence<I...>)
        {
            func(std::any_cast<Args>(args[I])...);
        }

        template <typename... Args>
        void Fire(Args&&... args)
        {
            if (connectedFunc)
                connectedFunc({ std::forward<Args>(args)... });
        }
    };

    // Incomplete
    struct TextButton
    {
        TextLabel label;
        EventListener* event;

        bool interactable = true;
        Color active, inactive;

        TextButton(TextLabel label, EventListener* event, bool interactable, Color active, Color inactive)
            : label(label), event(event), interactable(interactable), active(active), inactive(inactive) {}
    };

    struct Cam2D
    {
        Camera2D camera;
        std::vector<entt::entity> exclude;
    };

    struct Script 
    {
        std::function<void(lapCore::Scene*, entt::entity)> OnCreate;
        std::function<void(lapCore::Scene*, entt::entity, float)> OnUpdate;
        std::function<void(lapCore::Scene*, entt::entity)> OnDestroy;

        bool active = true;
        bool initiated = false;
    };

}

#endif