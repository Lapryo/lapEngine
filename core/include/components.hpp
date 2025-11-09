#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "raylib/raylib.h"
#include <functional>

namespace lapCore {
    class Scene; // <-- forward declaration
}

namespace lapCore
{
    struct Renderable
    {
        unsigned int zlayer;
        bool isScreenSpace;

        Renderable(unsigned int zlayer, bool isScreenSpace) : zlayer(zlayer), isScreenSpace(isScreenSpace) {}
    };

    struct Transform2D
    {
        Vector2 position, velocity, scale;
        float rotation;
    };

    struct Sprite : Renderable
    {
        Texture2D* texture = nullptr;
        std::string texName;
        Color tint;

        Sprite(const std::string &name, Color tint, unsigned int zlayer, bool isScreenSpace) : Renderable(zlayer, isScreenSpace), tint(tint), texName(name) {}
    };

    struct RectVisualizer : Renderable
    {
        Vector2 size;
        Color tint;

        RectVisualizer(Vector2 size, Color tint, unsigned int zlayer, bool isScreenSpace) : Renderable(zlayer, isScreenSpace), size(size), tint(tint) {}
    };

    struct TextLabel : Renderable
    {
        std::string text;
        Vector2 textPosition;
        float textSize;
        Color textColor;
        
        TextLabel(std::string text, Vector2 textPosition, float textSize, Color textColor, unsigned int zlayer, bool isScreenSpace) : Renderable(zlayer, isScreenSpace), text(text), textPosition(textPosition), textSize(textSize), textColor(textColor) {}
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
    };

}

#endif