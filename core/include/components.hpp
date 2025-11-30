#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "eutil.hpp"
#include "event.hpp"

#include <functional>
#include <any>

namespace lapCore
{
    class Scene; // <-- forward declaration
}

namespace lapCore
{
    struct Origin2D
    {
        rl::Vector2 position, scale;
    };

    struct Physics2D
    {
        rl::Vector2 velocity;
        rl::Vector2 gravity;
    };

    struct RotationalData
    {
        rl::Vector2 anchor;
        float rotation;
    };

    // New version of RectVisualizer, the basis of all GUI components (needed for all of them)
    struct Frame
    {
        Renderable renderable;
        UIOrigin origin;

        Frame(Renderable renderable, UIOrigin origin)
            : renderable(renderable), origin(origin) {}
    };
    
    struct UIList
    {
        FrameVector scrollSize;
        FrameVector displaySize;

        Axis2D direction;

        bool hScrollRight = true;
        bool vScrollBottom = true;

        bool maskOutsideContent = true;

        float scrollOffset = 0.f;
        float scrollSpeed = 20.f; // in pixels

        UIList(FrameVector scrollSize, FrameVector displaySize, bool hScrollRight, bool vScrollBottom, bool maskOutsideContent, float scrollOffset, float scrollSpeed, Axis2D direction)
            : scrollSize(scrollSize), displaySize(displaySize), hScrollRight(hScrollRight), vScrollBottom(vScrollBottom), maskOutsideContent(maskOutsideContent),
              scrollOffset(scrollOffset), scrollSpeed(scrollSpeed), direction(direction) {}
    };

    struct Sprite // Uses Origin2D instead
    {
        Renderable renderable;
        std::string textureName;

        Sprite(Renderable renderable, std::string textureName)
            : renderable(renderable), textureName(textureName) {}
    };

    struct Image
    {
        Sprite sprite;
        UIOrigin origin;

        Image(Sprite sprite, UIOrigin origin)
            : sprite(sprite), origin(origin) {}
    };

    struct TextLabel
    {
        Frame frame;

        std::string text;
        float textSize;
        Alignment textAlignment;
        FrameVector textBounds;
        Padding textPadding;

        TextLabel(Frame frame, std::string text, float textSize, Alignment textAlignment, FrameVector textBounds, Padding textPadding)
            : frame(frame), text(text), textSize(textSize), textAlignment(textAlignment), textBounds(textBounds), textPadding(textPadding) {}
    };

    struct EventBus
    {
        std::unordered_map<std::string, std::string> events;
    };

    struct UIButton
    {
        EventBus events;
        bool active = true;

        UIOrigin bounds;
        bool mouseHovering = false;

        UIButton(EventBus buttonEvents, UIOrigin bounds, bool active = true)
            : events(buttonEvents), bounds(bounds), active(active) {}
    };

    struct Cam2D
    {
        rl::Camera2D camera;
        std::vector<entt::entity> exclude;
    };

    template <typename T>
    struct Attribute
    {
        std::string name;
        T value;

        Attribute(std::string name, T value)
            : name(name), value(value) {}
    };

    struct Script
    {
        std::string onCreateFunction, onUpdateFunction, onDestroyFunction;

        bool active = true;
        bool initiated = false;

        Script(std::string onCreateFunction, std::string onUpdateFunction, std::string onDestroyFunction)
            : onCreateFunction(onCreateFunction), onUpdateFunction(onUpdateFunction), onDestroyFunction(onDestroyFunction) {}
    };

}

#endif