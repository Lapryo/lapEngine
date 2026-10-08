#pragma once

#include "eutil.hpp"
#include "event.hpp"
#include "box2d/box2d.h"

#include <functional>
#include <any>

using namespace lapCore;
using namespace EUTIL::Render;
using namespace EUTIL::Render::UI;

namespace lapCore
{

    class Scene; // <-- forward declaration

    namespace ELEMENTS
    {
        namespace Render
        {
            struct Sprite
            {
                Renderable renderable{};
                Animated animated{};

                // Not serializable, for runtime only
                Rectangle sourceRect{0, 0, 0, 0}, destRect{0, 0, 0, 0};
                bool flipX, flipY, flipD = false;

                entt::id_type textureID{0};
            };

            namespace UI
            {
                struct UIFrame
                {
                    Renderable renderable{};
                    UITransform transform{};
                };

                struct UIList
                {
                    FrameVector scrollSize{};
                    FrameVector displaySize{};
                    ScrollSettings settings{};

                    UIListSpreading spreading{UIListSpreading::BUNCH_START};

                    float scrollOffset{0.f};
                };

                /*
                // TODO: Implement this
                struct UIGrid
                {
                    No Scrolling:
                    ---------------
                    |-------------|
                    ||   |   |   ||
                    |-------------|
                    ||   |   |   ||
                    |-------------|
                    ||   |   |   ||
                    |-------------|
                    ---------------

                    With Scrolling:
                    ---------------
                    |-------------|------
                    ||   |   |   |||    |
                    |-------------|------
                    ||   |   |   |||    |
                    |-------------|------
                    ||   |   |   |||    |
                    |-------------|------
                    ---------------

                    Scroll size essentially acts as a another layer to the absolute size of a frame, except its not visualized fully

                    FrameVector scrollSize{};
                    unsigned int rows, columns = 1;
                    ScrollSettings scrollSettings{};

                    // Not serializable, runtime only, readonly
                    FrameVector slotSize{};
                    Vector2 scrollOffset{0.f, 0.f};
                };
                */

                struct UIGradient
                {
                    std::vector<Color> colorPoints{};
                    float angle{0.f};
                };

                struct UIImage
                {
                    Sprite sprite{};
                    UITransform transform{};
                };

                struct UITextLabel
                {
                    UIFrame frame{};

                    std::string text{""};
                    entt::id_type fontID{0};
                    float fontSize{10.f};
                    float spacing{1.f};

                    Alignment alignment{};
                    Padding padding{};

                    // Not serializable, for runtime only, readonly
                    Rectangle textDrawRect{0, 0, 0, 0};
                };

                struct UIButtonEventCallbacks
                {
                    entt::id_type mouseHover{0}, mouseEnter{0}, mouseExit{0}, leftClick{0}, rightClick{0}, middleClick{0};
                };

                struct UIButton
                {
                    UIButtonEventCallbacks eventCallbacks{};
                    bool active{true};
                    UITransform bounds;

                    bool inheritsUI{false};

                    bool mouseHovering{false}, inUIList{false}; // not serialized, used for runtime data
                };
            }
        }

        namespace Physics
        {
            struct Transform2D
            {
                Vector2 scale{1.f, 1.f};
                Vector2 position{0.f, 0.f};
                float rotation{0.f};
            };

            struct Physics2D
            {
                b2BodyDef bodyDef = b2DefaultBodyDef();
                b2ShapeDef shapeDef = b2DefaultShapeDef();
                b2Polygon polygon{};
            };

            struct SoundPoint
            {
                entt::id_type audioID{0};
                bool isMusic{false}, positional{true}, active{true}, usesOwn{true};
                float cutoffDistance{100.f};
                Vector2 target{0.f, 0.f}; // Target should be set to objects positions such as the player

                // For runtime use only
                Music musicPlayback{};
                Sound soundPlayback{};
                bool playing{false};
                bool initialized{false};
                bool autoPlay{true};

                // Not serializeable
                std::function<void()> onStart, onPlay, onEnd;
            };
        }

        namespace Other
        {
            struct Script
            {
                std::vector<entt::id_type> onCreateFunctions{}, onUpdateFunctions{}, onDestroyFunctions{};

                bool active{true};
                bool initiated{false};
            };
        }
    }
}