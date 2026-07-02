#pragma once

#include "eutil.hpp"
#include "event.hpp"
#include "box2d/box2d.h"

#include <functional>
#include <any>

#include <reflection.hpp>

namespace lapCore
{
    class Scene; // <-- forward declaration

    struct Transform2D
    {
        Vector2 scale{1.f, 1.f};
        Vector2 position{0.f, 0.f};
        float rotation{0.f};
    };
    template <>
    struct JSON::Serializer<Transform2D>
    {
        static json to_json(const Transform2D& t, SerializeContext* ctx = nullptr)
        {
            return json{
                { "position" , JSON::Serializer<Vector2>::to_json(t.position) },
                { "scale"    , JSON::Serializer<Vector2>::to_json(t.scale)    },
                { "rotation" , t.rotation                                     }
            };
        }
        static void from_json(Transform2D& t, const json& j)
        {
            if (j.contains("position"))
                JSON::Serializer<Vector2>::from_json(t.position, j.at("position"));
            if (j.contains("scale"))
                JSON::Serializer<Vector2>::from_json(t.scale, j.at("scale"));

            t.rotation = j.value("rotation", t.rotation);
        }
    };

    struct Physics2D
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2Polygon polygon{};
    };
    template <>
    struct JSON::Serializer<Physics2D>
    {
        static json to_json(const Physics2D& p, SerializeContext* ctx = nullptr)
        {
            return json{
                { "body"    , JSON::Serializer<b2BodyDef>::to_json(p.bodyDef)   },
                { "shape"   , JSON::Serializer<b2ShapeDef>::to_json(p.shapeDef) },
                { "polygon" , JSON::Serializer<b2Polygon>::to_json(p.polygon)   }
            };
        }
        static void from_json(Physics2D& p, const json& j)
        {
            if (j.contains("body"))
                JSON::Serializer<b2BodyDef>::from_json(p.bodyDef, j.at("body"));
            if (j.contains("shape"))
                JSON::Serializer<b2ShapeDef>::from_json(p.shapeDef, j.at("shape"));
            if (j.contains("polygon"))
                JSON::Serializer<b2Polygon>::from_json(p.polygon, j.at("polygon"));
        }
    };

    struct UIFrame
    {
        Renderable renderable{};
        UIOrigin origin{};
    };
    template <>
    struct JSON::Serializer<UIFrame>
    {
        static json to_json(const UIFrame& f, SerializeContext* ctx = nullptr)
        {
            return json{
                { "renderable" , JSON::Serializer<Renderable>::to_json(f.renderable, ctx) },
                { "origin"     , JSON::Serializer<UIOrigin>::to_json(f.origin, ctx)       }
            };
        }
        static void from_json(UIFrame& f, const json& j)
        {
            if (j.contains("renderable"))
                JSON::Serializer<Renderable>::from_json(f.renderable, j.at("renderable"));
            if (j.contains("origin"))
                JSON::Serializer<UIOrigin>::from_json(f.origin, j.at("origin"));
        }
    };

    struct UIList
    {
        FrameVector scrollSize{};
        FrameVector displaySize{};
        ScrollSettings settings{};

        float scrollOffset{0.f};
    };
    template <>
    struct JSON::Serializer<UIList>
    {
        static json to_json(const UIList& l, SerializeContext* ctx = nullptr)
        {
            return json{
                { "scroll-size"   , JSON::Serializer<FrameVector>::to_json(l.scrollSize, ctx)  },
                { "display-size"  , JSON::Serializer<FrameVector>::to_json(l.displaySize, ctx) },
                { "settings"      , JSON::Serializer<ScrollSettings>::to_json(l.settings, ctx) },
                { "scroll-offset" , l.scrollOffset                                             }
            };
        }
        static void from_json(UIList& l, const json& j)
        {
            if (j.contains("scroll-size"))
                JSON::Serializer<FrameVector>::from_json(l.scrollSize, j.at("scroll-size"));
            if (j.contains("display-size"))
                JSON::Serializer<FrameVector>::from_json(l.displaySize, j.at("display-size"));

            if (j.contains("settings"))
                JSON::Serializer<ScrollSettings>::from_json(l.settings, j.at("settings"));
            l.scrollOffset = j.value("scroll-offset", l.scrollOffset);
        }
    };

    struct Sprite
    {
        Renderable renderable{};
        Animated animated{};

        // Not serializable, for runtime only
        Rectangle sourceRect{0, 0, 0, 0}, destRect{0, 0, 0, 0};
        bool flipX, flipY, flipD = false;

        entt::id_type textureID{0};
    };
    template <>
    struct JSON::Serializer<Sprite>
    {
        static json to_json(const Sprite& s, SerializeContext* ctx = nullptr)
        {
            return json{
                { "renderable" , JSON::Serializer<Renderable>::to_json(s.renderable) },
                { "animated"   , JSON::Serializer<Animated>::to_json(s.animated)     },
                { "source-rect", JSON::Serializer<Rectangle>::to_json(s.sourceRect)  },
                { "dest-rect"  , JSON::Serializer<Rectangle>::to_json(s.destRect)    },
                { "flip-horizontal", s.flipX },
                { "flip-vertical", s.flipY },
                { "flip-diagonal", s.flipD },
                { "texture"    , ctx->resources->textures.GetName(s.textureID)       }
            };
        }
        static void from_json(Sprite& s, const json& j)
        {
            if (j.contains("renderable"))
                JSON::Serializer<Renderable>::from_json(s.renderable, j.at("renderable"));
            if (j.contains("animated"))
                JSON::Serializer<Animated>::from_json(s.animated, j.at("animated"));

            if (j.contains("source-rect"))
                JSON::Serializer<Rectangle>::from_json(s.sourceRect, j.at("source-rect"));
            if (j.contains("dest-rect"))
                JSON::Serializer<Rectangle>::from_json(s.destRect, j.at("dest-rect"));

            s.flipX = j.value("flip-horizontal", s.flipX);
            s.flipY = j.value("flip-vertical", s.flipY);
            s.flipD = j.value("flip-diagonal", s.flipD);
            
            if (j.contains("texture"))
                s.textureID = entt::hashed_string{j.at("texture").get<std::string>().c_str()}.value();
        }
    };

    struct UIImage
    {
        Sprite sprite{};
        UIOrigin origin{};
    };
    template <>
    struct JSON::Serializer<UIImage>
    {
        static json to_json(const UIImage& i, SerializeContext* ctx = nullptr)
        {
            return json{
                { "sprite" , JSON::Serializer<Sprite>::to_json(i.sprite, ctx)   },
                { "origin" , JSON::Serializer<UIOrigin>::to_json(i.origin, ctx) }
            };
        }
        static void from_json(UIImage& i, const json& j)
        {
            if (j.contains("sprite"))
                JSON::Serializer<Sprite>::from_json(i.sprite, j.at("sprite"));
            if (j.contains("origin"))
                JSON::Serializer<UIOrigin>::from_json(i.origin, j.at("origin"));
        }
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
    template <>
    struct JSON::Serializer<UITextLabel>
    {
        static json to_json(const UITextLabel& l, SerializeContext* ctx = nullptr)
        {
            return json{
                { "frame"     , JSON::Serializer<UIFrame>::to_json(l.frame, ctx)       },
                { "text"      , l.text                                                 },
                { "font"      , ctx->resources->fonts.GetName(l.fontID)                },
                { "font-size" , l.fontSize                                             },
                { "alignment" , JSON::Serializer<Alignment>::to_json(l.alignment, ctx) },
                { "padding"   , JSON::Serializer<Padding>::to_json(l.padding, ctx)     },
                { "spacing"   , l.spacing                                              }
            };
        }
        static void from_json(UITextLabel& l, const json& j)
        {
            if (j.contains("frame"))
                JSON::Serializer<UIFrame>::from_json(l.frame, j.at("frame"));
            l.text = j.value("text", l.text);
            if (j.contains("font"))
                l.fontID = entt::hashed_string{j.at("font").get<std::string>().c_str()}.value();
            l.fontSize = j.value("font-size", l.fontSize);
            l.spacing = j.value("spacing", l.spacing);
            
            if (j.contains("alignment"))
                JSON::Serializer<Alignment>::from_json(l.alignment, j.at("alignment"));
            if (j.contains("padding"))
                JSON::Serializer<Padding>::from_json(l.padding, j.at("padding"));
        }
    };

    struct UIButtonEventCallbacks
    {
        entt::id_type mouseHover{0}, mouseEnter{0}, mouseExit{0}, leftClick{0}, rightClick{0}, middleClick{0};
    };
    template <>
    struct JSON::Serializer<UIButtonEventCallbacks>
    {
        static json to_json(const UIButtonEventCallbacks& c, SerializeContext* ctx = nullptr)
        {
            return json{
                { "mouse-hover"  , EventRegistry::reverseLookup[c.mouseHover]  },
                { "mouse-enter"  , EventRegistry::reverseLookup[c.mouseEnter]  },
                { "mouse-exit"   , EventRegistry::reverseLookup[c.mouseExit]   },
                { "left-click"   , EventRegistry::reverseLookup[c.leftClick]   },
                { "middle-click" , EventRegistry::reverseLookup[c.middleClick] },
                { "right-click"  , EventRegistry::reverseLookup[c.rightClick]  }
            };
        }
        static void from_json(UIButtonEventCallbacks& c, const json& j)
        {
            if (j.contains("mouse-hover"))
                c.mouseHover  = entt::hashed_string{j.at("mouse-hover").get<std::string>().c_str()}.value();
            if (j.contains("mouse-enter"))
                c.mouseEnter  = entt::hashed_string{j.at("mouse-enter").get<std::string>().c_str()}.value();
            if (j.contains("mouse-exit"))
                c.mouseExit   = entt::hashed_string{j.at("mouse-exit").get<std::string>().c_str()}.value();
            if (j.contains("left-click"))
                c.leftClick   = entt::hashed_string{j.at("left-click").get<std::string>().c_str()}.value();
            if (j.contains("middle-click"))
                c.middleClick = entt::hashed_string{j.at("middle-click").get<std::string>().c_str()}.value();
            if (j.contains("right-click"))
                c.rightClick  = entt::hashed_string{j.at("right-click").get<std::string>().c_str()}.value();
        }
    };

    struct UIButton
    {
        UIButtonEventCallbacks eventCallbacks{};
        bool active{true};
        UIOrigin bounds{};

        bool inheritsListVisibility{false};

        bool mouseHovering{false}, inUIList{false}; // not serialized, used for runtime data
    };
    template <>
    struct JSON::Serializer<UIButton>
    {
        static json to_json(const UIButton& b, SerializeContext* ctx = nullptr)
        {
            return json{
                { "event-callbacks"          , JSON::Serializer<UIButtonEventCallbacks>::to_json(b.eventCallbacks, ctx) },
                { "bounds"                   , JSON::Serializer<UIOrigin>::to_json(b.bounds, ctx)                       },
                { "inherits-list-visibility" , b.inheritsListVisibility                                                 },
                { "active"                   , b.active                                                                 }
            };
        }
        static void from_json(UIButton& b, const json& j)
        {
            if (j.contains("event-callbacks"))
                JSON::Serializer<UIButtonEventCallbacks>::from_json(b.eventCallbacks, j.at("event-callbacks"));
            if (j.contains("bounds"))
                JSON::Serializer<UIOrigin>::from_json(b.bounds, j.at("bounds"));

            b.inheritsListVisibility = j.value("inherits-list-visibility", b.inheritsListVisibility);
            b.active                 = j.value("active", b.active);
        }
    };

    struct Script
    {
        std::vector<entt::id_type> onCreateFunctions{}, onUpdateFunctions{}, onDestroyFunctions{};

        bool active{true};
        bool initiated{false};
    };
    template <>
    struct JSON::Serializer<Script>
    {
        static json to_json(const Script& s, SerializeContext* ctx = nullptr)
        {
            std::vector<std::string> onCreates;
            for (const auto& func : s.onCreateFunctions)
            {
                auto it = EventRegistry::reverseLookup.find(func);
                if (it != EventRegistry::reverseLookup.end())
                    onCreates.push_back(it->second);
            }

            std::vector<std::string> onUpdates;
            for (const auto& func : s.onUpdateFunctions)
            {
                auto it = EventRegistry::reverseLookup.find(func);
                if (it != EventRegistry::reverseLookup.end())
                    onUpdates.push_back(it->second);
            }

            std::vector<std::string> onDestroys;
            for (const auto& func : s.onDestroyFunctions)
            {
                auto it = EventRegistry::reverseLookup.find(func);
                if (it != EventRegistry::reverseLookup.end())
                    onDestroys.push_back(it->second);
            }

            return json{
                { "functions"   ,
                {   { "create"  , onCreates  },
                    { "update"  , onUpdates  },
                    { "destroy" , onDestroys }  }             },
                { "active"      , s.active                    },
                { "initiated"   , s.initiated                 }
            };
        }
        static void from_json(Script& s, const json& j)
        {
            if (j.contains("functions"))
            {
                auto j_functions = j.at("functions");
                std::vector<entt::id_type> onCreates, onUpdates, onDestroys;
                if (j_functions.contains("create"))
                    for (const auto& funcName : j_functions.at("create"))
                        onCreates.push_back(HASH_ID(funcName.get<std::string>().c_str()));
                if (j_functions.contains("update"))
                    for (const auto& funcName : j_functions.at("update"))
                        onUpdates.push_back(HASH_ID(funcName.get<std::string>().c_str()));
                if (j_functions.contains("destroy"))
                    for (const auto& funcName : j_functions.at("destroy"))
                        onDestroys.push_back(HASH_ID(funcName.get<std::string>().c_str()));

                s.onCreateFunctions = onCreates;
                s.onUpdateFunctions = onUpdates;
                s.onDestroyFunctions = onDestroys;
            }

            s.active    = j.value("active", s.active);
            s.initiated = j.value("initiated", s.initiated);
        }
    };
}