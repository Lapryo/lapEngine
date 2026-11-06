#pragma once

#include "eutil.hpp"
#include "event.hpp"
#include "box2d/box2d.h"

#include <functional>
#include <any>
#include <variant>

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
            /**
             * @brief A sprite element is used to display an image/texture as a physical object within the scene,
             * different from the UIImage element.
             */
            struct Sprite
            {
                Renderable renderable{}; ///< Used for rendering the sprite
                Animated animated{}; ///< Used for animation settings for the sprite

                // Not serializable, for runtime only
                Rectangle sourceRect{0, 0, 0, 0}, destRect{0, 0, 0, 0}; ///< Used for animations primarily, other uses are possible
                bool flipX, flipY, flipD = false; ///< Flags for flipping the sprite horizontally, vertically, and diagonally

                id_type textureID{0}; ///< The ID of the texture to be used for the sprite
            };

            namespace UI
            {
                /**
                 * @brief A UIFrame element is used to display a rectangle onto the screen.
                 * This element is essentially the core, or base, of a UI hierarchy.
                 * Primarily used as either a container, or simply a visual element.
                 */
                struct UIFrame
                {
                    Renderable renderable{}; ///< Used for rendering the UIFrame
                    UITransform transform{}; ///< Used to transform the dimensions of the UIFrame
                };

                /**
                 * @brief A UIList element is used to display a list of objects containing UI elements in a vertical or horizontal layout.
                 * The scrolling behavior depends on the specified settings and the scroll size.
                 */
                struct UIList
                {
                    FrameVector scrollSize{}; ///< Used to define the scrolling area of the UIList, relative to that of the sibling UIFrame
                    FrameVector displaySize{}; ///< Used to define how much of an element in the UIList takes up the scrolling area
                    ScrollSettings settings{}; ///< Used to define the behavior of the scrolling

                    UIListSpreading spreading{UIListSpreading::BUNCH_START}; ///< Used to define how elements are spread within the UIList

                    float scrollOffset{0.f}; ///< Used to define the offset of the scrolling
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
                    vector<Color> colorPoints{};
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

                    string text{""};
                    id_type fontID{0};
                    float fontSize{10.f};
                    float spacing{1.f};

                    Alignment alignment{};
                    Padding padding{};

                    // Not serializable, for runtime only, readonly
                    Rectangle textDrawRect{0, 0, 0, 0};
                };

                struct UIButtonEventCallbacks
                {
                    id_type mouseHover{0}, mouseEnter{0}, mouseExit{0}, leftClick{0}, rightClick{0}, middleClick{0};
                };

                struct UIButton
                {
                    UIButtonEventCallbacks eventCallbacks{};
                    bool active{true};
                    UITransform bounds;

                    bool inheritsUI{false};
                    bool inheritedActive{true};
                    bool listActive{true};

                    bool mouseHovering{false}, inUIList{false}; // not serialized, used for runtime data
                };

                inline bool isUIButtonActive(const UIButton &b)
                {
                    return b.active && b.inheritedActive && (!b.inUIList || !b.inheritsUI || b.listActive);
                }
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
                id_type audioID{0};
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
                function<void()> onStart, onPlay, onEnd;
            };
        }

        namespace Other
        {
            struct Tags
            {
                unordered_set<id_type> tags;
            };

            using ParameterValue = variant<
                bool,
                int,
                float,
                string
            >;

            struct Parameter
            {
                id_type id{0};
                ParameterValue value{false};

                bool active{true};
            };

            struct Parameters
            {
                vector<Parameter> parameters;
            };

            struct Script
            {
                vector<id_type> onCreateFunctions{}, onUpdateFunctions{}, onDestroyFunctions{};

                bool active{true};
                bool initiated{false};
            };
        }
    }
}