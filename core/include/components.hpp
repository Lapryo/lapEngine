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
        bool visible = true;
        Color tint;

        Renderable(unsigned int zlayer, bool isScreenSpace) : zlayer(zlayer), isScreenSpace(isScreenSpace) {}
    };

    enum class Alignment
    {
        LEFT,
        MIDDLE,
        RIGHT
    };

    struct Origin
    {
        Vector2 position, scale;
        float rotation;
    };

    struct Physics2D : Origin
    {
        Vector2 velocity;
    };

    // New version of RectVisualizer
    struct Frame
    {
        Alignment horizontalAlignment;
        Alignment verticalAlignment;
        Vector2 anchor;

        Vector2 size;
        Vector2 padding;
        Color color;

        Frame(Alignment horizontal, Alignment vertical, Vector2 anchor, Vector2 size, Vector2 padding, Color color)
            : horizontalAlignment(horizontal), verticalAlignment(vertical), anchor(anchor), padding(padding), color(color) {}
    };

    struct /*[[deprecated("Use Origin and Physics2D instead, will not work")]]*/ Transform2D
    {
        Vector2 position, velocity, scale;
        float rotation;
    };

    struct Sprite : Renderable
    {
        Texture2D* texture = nullptr;
        std::string texName;
        Color tint;

        Sprite(Texture2D *texture, const std::string &name, Color &color, unsigned int zlayer, bool isScreenSpace) : Renderable(zlayer, isScreenSpace), texture(texture), texName(name), tint(color) {}
        Sprite(const std::string &name, Color tint, unsigned int zlayer, bool isScreenSpace) : Renderable(zlayer, isScreenSpace), tint(tint), texName(name) {}
    };

    struct /*[[deprecated("Use Frame instead")]]*/ RectVisualizer : Renderable
    {
        Vector2 offset;
        Vector2 size;
        Color tint;

        /*[[deprecated("Use frame instead")]]*/
        RectVisualizer(Vector2 offset, Vector2 size, Color tint, unsigned int zlayer, bool isScreenSpace) : Renderable(zlayer, isScreenSpace), offset(offset), size(size), tint(tint) {}
    };

    struct TextLabel : Renderable
    {
        std::string text;
        Vector2 offset;
        float size;
        Color color;
        
        TextLabel(std::string text, Vector2 offset, float size, Color color, unsigned int zlayer, bool isScreenSpace) : Renderable(zlayer, isScreenSpace), text(text), offset(offset), size(size), color(color) {}
    };

    // Incomplete
    struct TextButton : TextLabel
    {
        template <typename... Args>
        void SetConnectedFunc(std::function<void(Args...)> function)
        {
            connectedFuncWrapper = [function](auto&&... args) {
                function(std::forward<decltype(args)>(args)...);
            };
        }



    private:
        std::function<void()> connectedFuncWrapper;
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