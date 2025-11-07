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
        Texture2D* texture;
        Color tint;
        unsigned int zlayer;
        bool isScreenSpace;
    };

    struct RectVisualizer {
        Vector2 size;
        Color tint;
        bool isScreenSpace;
    };

    struct TextLabel
    {
        std::string text;
        Vector2 textPosition;
        float textSize;
        Color textColor;
        bool isScreenSpace;
    };

    struct Cam2D
    {
        Camera2D camera;
        std::vector<entt::entity> exclude;
    };
}

#endif