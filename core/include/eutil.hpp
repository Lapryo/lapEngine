#ifndef EUTIL_HPP
#define EUTIL_HPP

#include "raylib/raylib.h"

#include <string>
#include <vector>

namespace lapCore
{
    struct Renderable
    {
        Rectangle space;

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
        Vector2 scale;
        Vector2 offset;

        FrameVector(Vector2 scale, Vector2 offset)
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