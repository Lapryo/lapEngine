#ifndef EUTIL_HPP
#define EUTIL_HPP

#include "raylib/raylib_namespace.h"

#include <vector>
#include <string>

namespace lapCore
{
    struct Renderable
    {
        rl::Rectangle space;

        unsigned int zlayer;
        bool isScreenSpace;
        bool visible = true;
        rl::Color tint;

        Renderable(unsigned int zlayer, bool isScreenSpace, bool visible, rl::Color tint)
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

    struct Alignment
    {
        HorizontalAlignment horizontal;
        VerticalAlignment vertical;

        Alignment(HorizontalAlignment horizontal, VerticalAlignment vertical)
            : horizontal(horizontal), vertical(vertical) {}
    };

    enum class Axis2D
    {
        HORIZONTAL,
        VERTICAL
    };

    struct FrameVector
    {
        rl::Vector2 scale;
        rl::Vector2 offset;

        FrameVector(rl::Vector2 scale, rl::Vector2 offset)
            : scale(scale), offset(offset) {}
    };

    struct Padding
    {
        float top, bottom, left, right;

        Padding(float top, float bottom, float left, float right)
            : top(top), bottom(bottom), left(left), right(right) {}
    };

    struct UIOrigin
    {
        FrameVector position;
        FrameVector size;

        UIOrigin(FrameVector position, FrameVector size)
            : position(position), size(size) {}
    };

    class FileDialogs
    {
    public:
        static std::string OpenFile(std::vector<std::string> filters);
        static std::string SaveFile(std::vector<std::string> filters);
    };
}

#endif