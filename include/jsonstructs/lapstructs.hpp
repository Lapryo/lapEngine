#pragma once

#include "elements.hpp"

using namespace lapCore;
using namespace EUTIL;
using namespace ELEMENTS;

using namespace EUTIL::Render;
using namespace ELEMENTS::Render;

using namespace EUTIL::Render::UI;
using namespace ELEMENTS::Render::UI;

using namespace EUTIL::Other;
using namespace ELEMENTS::Other;

using namespace ELEMENTS::Physics;

using namespace Math;

namespace lapCore
{
    // EUTIL

    template <>
    struct JSON::Serializer<Renderable>
    {
        static json to_json(const Renderable& r, SerializeContext* ctx = nullptr)
        {
            return json{
                {"z-layer", r.zlayer},
                {"y-sort", r.ySort},
                {"is-screen-space", r.isScreenSpace},
                {"visible", r.visible},
                {"tint", JSON::Serializer<Color>::to_json(r.tint, ctx)},
                {"inherits-ui", r.inheritsUI},
                {"culling", r.culling}
            };
        }

        static void from_json(Renderable& r, const json& j)
        {
            r.zlayer = j.value("z-layer", r.zlayer);
            r.ySort = j.value("y-sort", r.ySort);
            r.isScreenSpace = j.value("is-screen-space", r.isScreenSpace);
            r.visible = j.value("visible", r.visible);
            r.inheritsUI = j.value("inherits-ui", r.inheritsUI);
            r.culling = j.value("culling", r.culling);

            if (j.contains("tint"))
                JSON::Serializer<Color>::from_json(r.tint, j.at("tint"));
        }
    };

    template <>
    struct JSON::Serializer<Axis2D>
    {
        static json to_json(const Axis2D& a, SerializeContext* ctx = nullptr)
        {
            switch (a)
            {
                case Axis2D::HORIZONTAL:
                    return "horizontal";
                default:
                    return "vertical";
            }
        }

        static void from_json(Axis2D& a, const json& j)
        {
            const auto val = j.get<std::string>();

            if (val == "horizontal")
                a = Axis2D::HORIZONTAL;
            else
                a = Axis2D::VERTICAL;
        }
    };

    template <>
    struct JSON::Serializer<ScrollSettings>
    {
        static json to_json(const ScrollSettings& s, SerializeContext* ctx = nullptr)
        {
            return json{
                {"free-mode", s.freeMode},
                {"direction", JSON::Serializer<Axis2D>::to_json(s.direction, ctx)},
                {"horizontal-scrolling", s.allowHorizontalScrolling},
                {"vertical-scrolling", s.allowVerticalScrolling},
                {"scroll-speed", s.scrollSpeed}
            };
        }

        static void from_json(ScrollSettings& s, const json& j)
        {
            if (j.contains("direction"))
                JSON::Serializer<Axis2D>::from_json(s.direction, j.at("direction"));

            s.freeMode = j.value("free-mode", s.freeMode);

            s.allowHorizontalScrolling = j.value("horizontal-scrolling", s.allowHorizontalScrolling);
            s.allowVerticalScrolling = j.value("vertical-scrolling", s.allowVerticalScrolling);
            s.scrollSpeed = j.value("scroll-speed", s.scrollSpeed);
        }
    };

    template <>
    struct JSON::Serializer<Animated>
    {
        static json to_json(const Animated& a, SerializeContext* ctx = nullptr)
        {
            return json{
                {"active", a.active},
                {"index", a.index},
                {"columns", a.columns},
                {"frame-times", a.frameTimes}
            };
        }
        static void from_json(Animated& a, const json& j)
        {
            a.active = j.value("active", a.active);
            a.index = j.value("index", a.index);
            a.columns = j.value("columns", a.columns);
            a.frameTimes = j.value("frame-times", a.frameTimes);
        }
    };

    template <>
    struct JSON::Serializer<HorizontalAlignment>
    {
        static json to_json(const HorizontalAlignment& h, SerializeContext* ctx = nullptr)
        {
            switch (h)
            {
                case HorizontalAlignment::MIDDLE:
                    return "middle";
                case HorizontalAlignment::RIGHT:
                    return "right";
                default:
                    return "left";
            }
        }

        static void from_json(HorizontalAlignment& h, const json& j)
        {
            const auto val = j.get<std::string>();

            if (val == "left")
                h = HorizontalAlignment::LEFT;
            else if (val == "middle")
                h = HorizontalAlignment::MIDDLE;
            else
                h = HorizontalAlignment::RIGHT;
        }
    };

    template <>
    struct JSON::Serializer<VerticalAlignment>
    {
        static json to_json(const VerticalAlignment& v, SerializeContext* ctx = nullptr)
        {
            switch (v)
            {
                case VerticalAlignment::MIDDLE:
                    return "middle";
                case VerticalAlignment::TOP:
                    return "bottom";
                default:
                    return "top";
            }
        }

        static void from_json(VerticalAlignment& v, const json& j)
        {
            const auto val = j.get<std::string>();

            if (val == "top")
                v = VerticalAlignment::TOP;
            else if (val == "middle")
                v = VerticalAlignment::MIDDLE;
            else
                v = VerticalAlignment::BOTTOM;
        }
    };

    template <>
    struct JSON::Serializer<Direction2D>
    {
        static json to_json(const Direction2D& d, SerializeContext* ctx = nullptr)
        {
            switch (d)
            {
                case Direction2D::UP:
                    return "up";
                case Direction2D::DOWN:
                    return "down";
                case Direction2D::LEFT:
                    return "left";
                default:
                    return "right";
            }
        }

        static void from_json(Direction2D& d, const json& j)
        {
            const auto val = j.get<std::string>();

            if (val == "up")
                d = Direction2D::UP;
            else if (val == "down")
                d = Direction2D::DOWN;
            else if (val == "left")
                d = Direction2D::LEFT;
            else
                d = Direction2D::RIGHT;
        }
    };

    template <>
    struct JSON::Serializer<Alignment>
    {
        static json to_json(const Alignment& a, SerializeContext* ctx = nullptr)
        {
            return json{
                {"horizontal", JSON::Serializer<HorizontalAlignment>::to_json(a.horizontal, ctx)},
                {"vertical", JSON::Serializer<VerticalAlignment>::to_json(a.vertical, ctx)}
            };
        }

        static void from_json(Alignment& a, const json& j)
        {
            if (j.contains("horizontal"))
                JSON::Serializer<HorizontalAlignment>::from_json(a.horizontal, j.at("horizontal"));
            if (j.contains("vertical"))
                JSON::Serializer<VerticalAlignment>::from_json(a.vertical, j.at("vertical"));
        }
    };

    template <>
    struct JSON::Serializer<FrameVector>
    {
        static json to_json(const FrameVector& v, SerializeContext* ctx = nullptr)
        {
            return json{
                {"scale", JSON::Serializer<Vector2>::to_json(v.scale, ctx)},
                {"offset", JSON::Serializer<Vector2>::to_json(v.offset, ctx)}
            };
        }

        static void from_json(FrameVector& v, const json& j)
        {
            if (j.contains("scale"))
                JSON::Serializer<Vector2>::from_json(v.scale, j.at("scale"));
            if (j.contains("offset"))
                JSON::Serializer<Vector2>::from_json(v.offset, j.at("offset"));
        }
    };

    template <>
    struct JSON::Serializer<Padding>
    {
        static json to_json(const Padding& p, SerializeContext* ctx = nullptr)
        {
            return json{
                {"top", p.top},
                {"bottom", p.bottom},
                {"left", p.left},
                {"right", p.right}
            };
        }

        static void from_json(Padding& p, const json& j)
        {
            p.top = j.value("top", 0.f);
            p.bottom = j.value("bottom", 0.f);
            p.left = j.value("left", 0.f);
            p.right = j.value("right", 0.f);
        }
    };

    template <>
    struct JSON::Serializer<UITransform>
    {
        static json to_json(const UITransform& t, SerializeContext* ctx = nullptr)
        {
            return json{
                {"position", JSON::Serializer<FrameVector>::to_json(t.position, ctx)},
                {"size", JSON::Serializer<FrameVector>::to_json(t.size, ctx)},
                {"anchor", JSON::Serializer<FrameVector>::to_json(t.anchor, ctx)},
                {"rotation", t.rotation}
            };
        }

        static void from_json(UITransform& t, const json& j)
        {
            if (j.contains("position"))
                JSON::Serializer<FrameVector>::from_json(t.position, j.at("position"));
            if (j.contains("size"))
                JSON::Serializer<FrameVector>::from_json(t.size, j.at("size"));
            if (j.contains("anchor"))
                JSON::Serializer<FrameVector>::from_json(t.anchor, j.at("anchor"));

            t.rotation = j.value("rotation", t.rotation);
        }
    };

    // ELEMENTS

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

    template <>
    struct JSON::Serializer<UIFrame>
    {
        static json to_json(const UIFrame& f, SerializeContext* ctx = nullptr)
        {
            return json{
                { "renderable" , JSON::Serializer<Renderable>::to_json(f.renderable, ctx) },
                { "transform"  , JSON::Serializer<UITransform>::to_json(f.transform, ctx) }
            };
        }
        static void from_json(UIFrame& f, const json& j)
        {
            if (j.contains("renderable"))
                JSON::Serializer<Renderable>::from_json(f.renderable, j.at("renderable"));
            if (j.contains("transform"))
                JSON::Serializer<UITransform>::from_json(f.transform, j.at("transform"));
        }
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

    template <>
    struct JSON::Serializer<UIImage>
    {
        static json to_json(const UIImage& i, SerializeContext* ctx = nullptr)
        {
            return json{
                { "sprite" , JSON::Serializer<Sprite>::to_json(i.sprite, ctx)   },
                { "transform"  , JSON::Serializer<UITransform>::to_json(i.transform, ctx) }
            };
        }
        static void from_json(UIImage& i, const json& j)
        {
            if (j.contains("sprite"))
                JSON::Serializer<Sprite>::from_json(i.sprite, j.at("sprite"));
            if (j.contains("transform"))
                JSON::Serializer<UITransform>::from_json(i.transform, j.at("transform"));
        }
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

    template <>
    struct JSON::Serializer<UIButton>
    {
        static json to_json(const UIButton& b, SerializeContext* ctx = nullptr)
        {
            return json{
                { "event-callbacks"          , JSON::Serializer<UIButtonEventCallbacks>::to_json(b.eventCallbacks, ctx) },
                { "bounds"                   , JSON::Serializer<UITransform>::to_json(b.bounds, ctx)                       },
                { "inherits-ui" , b.inheritsUI                                               },
                { "active"                   , b.active                                                                 }
            };
        }
        static void from_json(UIButton& b, const json& j)
        {
            if (j.contains("event-callbacks"))
                JSON::Serializer<UIButtonEventCallbacks>::from_json(b.eventCallbacks, j.at("event-callbacks"));
            if (j.contains("bounds"))
                JSON::Serializer<UITransform>::from_json(b.bounds, j.at("bounds"));

            b.inheritsUI = j.value("inherits-list-visibility", b.inheritsUI);
            b.active                 = j.value("active", b.active);
        }
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