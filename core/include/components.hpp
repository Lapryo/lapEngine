#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "raylib/raylib.h"

namespace lapCore
{
    struct Transform2D {
        Vector2 position, velocity, scale;
        float rotation;
    };

    struct Sprite {
        Texture2D texture;
        Rectangle sourceRec;
        Vector2 origin;
        float rotation;
        Color tint;
    };
}

#endif