#ifndef EUTIL_HPP
#define EUTIL_HPP

#include "raylib/raylib.h"

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
}

#endif