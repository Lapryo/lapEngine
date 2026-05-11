#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "eutil.hpp"
#include "event.hpp"
#include "box2d/box2d.h"

#include <functional>
#include <any>

namespace lapCore
{
    class Scene; // <-- forward declaration
}

namespace lapCore
{
    // Position is being constantly updated by the physics system, scale can be changed freely
    struct Origin2D
    {
        Vector2 position, scale;
        float rotation;
    };

    struct Physics2D
    {
        Vector2 initialPosition;

        b2BodyDef bodyDef;
        b2ShapeDef shapeDef;
        b2Polygon polygon;
        b2BodyId bodyID;
    };

    // Likely to be removed
    struct Rotation2D
    {
        Vector2 anchor;
        float rotation;
    };

    // New version of RectVisualizer, the basis of all GUI components (needed for all of them)
    struct Frame
    {
        Renderable renderable;
        UIOrigin origin;

        Frame() {}
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

        UIList() {}
        UIList(FrameVector scrollSize, FrameVector displaySize, bool hScrollRight, bool vScrollBottom, bool maskOutsideContent, float scrollOffset, float scrollSpeed, Axis2D direction)
            : scrollSize(scrollSize), displaySize(displaySize), hScrollRight(hScrollRight), vScrollBottom(vScrollBottom), maskOutsideContent(maskOutsideContent),
              scrollOffset(scrollOffset), scrollSpeed(scrollSpeed), direction(direction) {}
    };

    struct Sprite // Uses Origin2D instead
    {
        Renderable renderable;
        std::string textureName;

        Sprite() {}
        Sprite(Renderable renderable, std::string textureName)
            : renderable(renderable), textureName(textureName) {}
    };

    struct lapImage
    {
        Sprite sprite;
        UIOrigin origin;

        lapImage() {}
        lapImage(Sprite sprite, UIOrigin origin)
            : sprite(sprite), origin(origin) {}
    };

    struct TextLabel
    {
        Frame frame;

        std::string text;
        float fontSize;
        Alignment textAlignment;
        FrameVector textBounds;
        Padding textPadding;

        TextLabel() {}
        TextLabel(Frame frame, std::string text, float textSize, Alignment textAlignment, FrameVector textBounds, Padding textPadding)
            : frame(frame), text(text), fontSize(textSize), textAlignment(textAlignment), textBounds(textBounds), textPadding(textPadding) {}
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

        bool usesListVisibility = false;

        UIButton() {}
        UIButton(EventBus buttonEvents, UIOrigin bounds, bool active = true, bool usesListVisibility = false)
            : events(buttonEvents), bounds(bounds), active(active), usesListVisibility(usesListVisibility) {}
    };

    struct Cam2D
    {
        Camera2D camera;
        std::vector<std::string> exclude;
    };

    template <typename T>
    struct Attribute
    {
        std::string name;
        T value;

        Attribute() {}
        Attribute(std::string name, T value)
            : name(name), value(value) {}
    };

    struct Script
    {
        std::string onCreateFunction, onUpdateFunction, onDestroyFunction;

        bool active = true;
        bool initiated = false;

        Script() {}
        Script(std::string onCreateFunction, std::string onUpdateFunction, std::string onDestroyFunction)
            : onCreateFunction(onCreateFunction), onUpdateFunction(onUpdateFunction), onDestroyFunction(onDestroyFunction) {}
    };

}

#endif