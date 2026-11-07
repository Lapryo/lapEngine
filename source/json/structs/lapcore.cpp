/*
TODO LATER-DATE:
1. Rewrite the comments for the examples
2. Rewrite the comments for the examples
3. Fully comment the code before release
4. Fix the CMakeLists to make the builds for the examples in their directories,
rather than the overall build directory

WISHLIST:
1. Snapshot functionality
2. Full console support (including handhelds like Anbernic, etc.)
3. Add debug rendering for physics system and render system overall
4. Performance checker (with time ranges and everything)

*/

#include "reflection.hpp"
#include "json/structs/lapcore.hpp"

#include "resourcemanager.hpp"

using namespace std;

namespace lapCore::JSON
{
    // EUTIL

    // RENDERABLE
    json Serializer<Renderable>::to_json(const Renderable& r, SerializeContext* ctx)
    {
        return json{
            {"z-layer", r.zlayer},
            {"y-sort", r.ySort},
            {"is-screen-space", r.isScreenSpace},
            {"visible", r.visible},
            {"tint", Serializer<Color>::to_json(r.tint, ctx)},
            {"inherits-ui", r.inheritsUI},
            {"culling", r.culling}
        };
    }
    void Serializer<Renderable>::from_json(Renderable& r, const json& j)
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

    // AXIS2D
    json Serializer<Axis2D>::to_json(const Axis2D& a, SerializeContext* ctx)
    {
        switch (a)
        {
            case Axis2D::HORIZONTAL:
                return "horizontal";
            default:
                return "vertical";
        }
    }
    void Serializer<Axis2D>::from_json(Axis2D& a, const json& j)
    {
        const auto val = j.get<string>();

        if (val == "horizontal")
            a = Axis2D::HORIZONTAL;
        else
            a = Axis2D::VERTICAL;
    }

    // SCROLL SETTINGS
    json Serializer<ScrollSettings>::to_json(const ScrollSettings& s, SerializeContext* ctx)
    {
        return json{
            {"free-mode", s.freeMode},
            {"direction", JSON::Serializer<Axis2D>::to_json(s.direction, ctx)},
            {"horizontal-scrolling", s.allowHorizontalScrolling},
            {"vertical-scrolling", s.allowVerticalScrolling},
            {"scroll-speed", s.scrollSpeed}
        };
    }

    void Serializer<ScrollSettings>::from_json(ScrollSettings& s, const json& j)
    {
        if (j.contains("direction"))
            JSON::Serializer<Axis2D>::from_json(s.direction, j.at("direction"));

        s.freeMode = j.value("free-mode", s.freeMode);

        s.allowHorizontalScrolling = j.value("horizontal-scrolling", s.allowHorizontalScrolling);
        s.allowVerticalScrolling = j.value("vertical-scrolling", s.allowVerticalScrolling);
        s.scrollSpeed = j.value("scroll-speed", s.scrollSpeed);
    }

    // ANIMATED
    json Serializer<Animated>::to_json(const Animated& a, SerializeContext* ctx)
    {
        return json{
            {"active", a.active},
            {"index", a.index},
            {"columns", a.columns},
            {"frame-times", a.frameTimes}
        };
    }
    void Serializer<Animated>::from_json(Animated& a, const json& j)
    {
        a.active = j.value("active", a.active);
        a.index = j.value("index", a.index);
        a.columns = j.value("columns", a.columns);
        a.frameTimes = j.value("frame-times", a.frameTimes);
    }

    // HORIZONTAL ALIGNMENT
    json Serializer<HorizontalAlignment>::to_json(const HorizontalAlignment& h, SerializeContext* ctx)
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
    void Serializer<HorizontalAlignment>::from_json(HorizontalAlignment& h, const json& j)
    {
        const auto val = j.get<string>();

        if (val == "left")
            h = HorizontalAlignment::LEFT;
        else if (val == "middle")
            h = HorizontalAlignment::MIDDLE;
        else
            h = HorizontalAlignment::RIGHT;
    }

    // VERTICAL ALIGNMENT
    json Serializer<VerticalAlignment>::to_json(const VerticalAlignment& v, SerializeContext* ctx)
    {
        switch (v)
        {
            case VerticalAlignment::MIDDLE:
                return "middle";
            case VerticalAlignment::TOP:
                return "top";
            default:
                return "bottom";
        }
    }
    void Serializer<VerticalAlignment>::from_json(VerticalAlignment& v, const json& j)
    {
        const auto val = j.get<string>();

        if (val == "top")
            v = VerticalAlignment::TOP;
        else if (val == "middle")
            v = VerticalAlignment::MIDDLE;
        else
            v = VerticalAlignment::BOTTOM;
    }

    // DIRECTION2D
    json Serializer<Direction2D>::to_json(const Direction2D& d, SerializeContext* ctx)
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
    void Serializer<Direction2D>::from_json(Direction2D& d, const json& j)
    {
        const auto val = j.get<string>();

        if (val == "up")
            d = Direction2D::UP;
        else if (val == "down")
            d = Direction2D::DOWN;
        else if (val == "left")
            d = Direction2D::LEFT;
        else
            d = Direction2D::RIGHT;
    }

    // ALIGNMENT
    json Serializer<Alignment>::to_json(const Alignment& a, SerializeContext* ctx)
    {
        return json{
            {"horizontal", JSON::Serializer<HorizontalAlignment>::to_json(a.horizontal, ctx)},
            {"vertical", JSON::Serializer<VerticalAlignment>::to_json(a.vertical, ctx)}
        };
    }

    void Serializer<Alignment>::from_json(Alignment& a, const json& j)
    {
        if (j.contains("horizontal"))
            JSON::Serializer<HorizontalAlignment>::from_json(a.horizontal, j.at("horizontal"));
        if (j.contains("vertical"))
            JSON::Serializer<VerticalAlignment>::from_json(a.vertical, j.at("vertical"));
    }

    // FRAME VECTOR
    json Serializer<FrameVector>::to_json(const FrameVector& v, SerializeContext* ctx)
    {
        return json{
            {"scale", JSON::Serializer<Vector2>::to_json(v.scale, ctx)},
            {"offset", JSON::Serializer<Vector2>::to_json(v.offset, ctx)}
        };
    }
    void Serializer<FrameVector>::from_json(FrameVector& v, const json& j)
    {
        if (j.contains("scale"))
            JSON::Serializer<Vector2>::from_json(v.scale, j.at("scale"));
        if (j.contains("offset"))
            JSON::Serializer<Vector2>::from_json(v.offset, j.at("offset"));
    }

    // PADDING
    json Serializer<Padding>::to_json(const Padding& p, SerializeContext* ctx)
    {
        return json{
            {"top", p.top},
            {"bottom", p.bottom},
            {"left", p.left},
            {"right", p.right}
        };
    }
    void Serializer<Padding>::from_json(Padding& p, const json& j)
    {
        p.top = j.value("top", 0.f);
        p.bottom = j.value("bottom", 0.f);
        p.left = j.value("left", 0.f);
        p.right = j.value("right", 0.f);
    }

    // UITRANSFORM
    json Serializer<UITransform>::to_json(const UITransform& t, SerializeContext* ctx)
    {
        return json{
            {"position", JSON::Serializer<FrameVector>::to_json(t.position, ctx)},
            {"size", JSON::Serializer<FrameVector>::to_json(t.size, ctx)},
            {"anchor", JSON::Serializer<FrameVector>::to_json(t.anchor, ctx)},
            {"rotation", t.rotation}
        };
    }

    void Serializer<UITransform>::from_json(UITransform& t, const json& j)
    {
        if (j.contains("position"))
            JSON::Serializer<FrameVector>::from_json(t.position, j.at("position"));
        if (j.contains("size"))
            JSON::Serializer<FrameVector>::from_json(t.size, j.at("size"));
        if (j.contains("anchor"))
            JSON::Serializer<FrameVector>::from_json(t.anchor, j.at("anchor"));

        t.rotation = j.value("rotation", t.rotation);
    }

    // ELEMENTS

    // TRANSFORM2D
    json Serializer<Transform2D>::to_json(const Transform2D& t, SerializeContext* ctx)
    {
        return json{
            { "position" , JSON::Serializer<Vector2>::to_json(t.position) },
            { "scale"    , JSON::Serializer<Vector2>::to_json(t.scale)    },
            { "rotation" , t.rotation                                     }
        };
    }
    void Serializer<Transform2D>::from_json(Transform2D& t, const json& j)
    {
        if (j.contains("position"))
            JSON::Serializer<Vector2>::from_json(t.position, j.at("position"));
        if (j.contains("scale"))
            JSON::Serializer<Vector2>::from_json(t.scale, j.at("scale"));

        t.rotation = j.value("rotation", t.rotation);
    }

    // PHYSICS2D
    json Serializer<Physics2D>::to_json(const Physics2D& p, SerializeContext* ctx)
    {
        return json{
            { "body"    , JSON::Serializer<b2BodyDef>::to_json(p.bodyDef)   },
            { "shape"   , JSON::Serializer<b2ShapeDef>::to_json(p.shapeDef) },
            { "polygon" , JSON::Serializer<b2Polygon>::to_json(p.polygon)   }
        };
    }
    void Serializer<Physics2D>::from_json(Physics2D& p, const json& j)
    {
        if (j.contains("body"))
            JSON::Serializer<b2BodyDef>::from_json(p.bodyDef, j.at("body"));
        if (j.contains("shape"))
            JSON::Serializer<b2ShapeDef>::from_json(p.shapeDef, j.at("shape"));
        if (j.contains("polygon"))
            JSON::Serializer<b2Polygon>::from_json(p.polygon, j.at("polygon"));
    }

    // SOUND POINT
    json Serializer<SoundPoint>::to_json(const SoundPoint& s, SerializeContext* ctx)
    {
        return json{
            {"is-music"       , s.isMusic},
            {"active"         , s.active},
            {"cutoff-distance", s.cutoffDistance},
            {"uses-own"       , s.usesOwn},
            {"auto-play"      , s.autoPlay},
            {"audio-id"       , SerializeEventID(s.audioID)},
            {"positional"     , s.positional},
            {"target"         , JSON::Serializer<Vector2>::to_json(s.target, ctx)}
        };
    }
    void Serializer<SoundPoint>::from_json(SoundPoint& s, const json& j)
    {
        s.isMusic = j.value("is-music", s.isMusic);
        if (j.contains("audio-id"))
            s.audioID = DeserializeEventID(j.at("audio-id"));
        
        s.cutoffDistance = j.value("cutoff-distance", s.cutoffDistance);
        s.active = j.value("active", s.active);
        s.usesOwn = j.value("uses-own", s.usesOwn);
        s.autoPlay = j.value("auto-play", s.autoPlay);

        s.positional = j.value("positional", s.positional);
        if (j.contains("target"))
            JSON::Serializer<Vector2>::from_json(s.target, j.at("target"));
    }

    // UIFRAME
    json Serializer<UIFrame>::to_json(const UIFrame& f, SerializeContext* ctx)
    {
        return json{
            { "renderable" , JSON::Serializer<Renderable>::to_json(f.renderable, ctx) },
            { "transform"  , JSON::Serializer<UITransform>::to_json(f.transform, ctx) }
        };
    }
    void Serializer<UIFrame>::from_json(UIFrame& f, const json& j)
    {
        if (j.contains("renderable"))
            JSON::Serializer<Renderable>::from_json(f.renderable, j.at("renderable"));
        if (j.contains("transform"))
            JSON::Serializer<UITransform>::from_json(f.transform, j.at("transform"));
    }

    // UILIST SPREADING
    json Serializer<UIListSpreading>::to_json(const UIListSpreading& s, SerializeContext* ctx)
    {
        switch (s)
        {
            case UIListSpreading::BUNCH_START:
                return "bunch-start";
            case UIListSpreading::BUNCH_MIDDLE:
                return "bunch-middle";
            case UIListSpreading::BUNCH_END:
                return "bunch-end";
            case UIListSpreading::EVENLY:
                return "evenly";
            case UIListSpreading::DISTANCING:
                return "distancing";
            default:
                return "bunch-start";
        }
    }
    void Serializer<UIListSpreading>::from_json(UIListSpreading& s, const json& j)
    {
        const auto& value = j.get<string>();
        if (value == "bunch-start")
            s = UIListSpreading::BUNCH_START;
        else if (value == "bunch-middle")
            s = UIListSpreading::BUNCH_MIDDLE;
        else if (value == "bunch-end")
            s = UIListSpreading::BUNCH_END;
        else if (value == "evenly")
            s = UIListSpreading::EVENLY;
        else if (value == "distancing")
            s = UIListSpreading::DISTANCING;
        else
            s = UIListSpreading::BUNCH_START;
    }

    // UILIST
    json Serializer<UIList>::to_json(const UIList& l, SerializeContext* ctx)
    {
        return json{
            { "scroll-size"   , JSON::Serializer<FrameVector>::to_json(l.scrollSize, ctx)    },
            { "display-size"  , JSON::Serializer<FrameVector>::to_json(l.displaySize, ctx)   },
            { "settings"      , JSON::Serializer<ScrollSettings>::to_json(l.settings, ctx)   },
            { "spreading"     , JSON::Serializer<UIListSpreading>::to_json(l.spreading, ctx) },
            { "scroll-offset" , l.scrollOffset                                               }
        };
    }
    void Serializer<UIList>::from_json(UIList& l, const json& j)
    {
        if (j.contains("scroll-size"))
            JSON::Serializer<FrameVector>::from_json(l.scrollSize, j.at("scroll-size"));
        if (j.contains("display-size"))
            JSON::Serializer<FrameVector>::from_json(l.displaySize, j.at("display-size"));
        if (j.contains("settings"))
            JSON::Serializer<ScrollSettings>::from_json(l.settings, j.at("settings"));
        if (j.contains("spreading"))
            JSON::Serializer<UIListSpreading>::from_json(l.spreading, j.at("spreading"));

        l.scrollOffset = j.value("scroll-offset", l.scrollOffset);
    }

    // UIGRADIENT
    json Serializer<UIGradient>::to_json(const UIGradient& g, SerializeContext* ctx)
    {
        vector<vector<unsigned char>> colorPts;
        for (const auto& color : g.colorPoints)
            colorPts.push_back({color.r, color.g, color.b, color.a});

        return json{
            { "colors-points"    , colorPts },
            { "angle"       , g.angle }
        };
    }
    void Serializer<UIGradient>::from_json(UIGradient& g, const json& j)
    {
        g.colorPoints.clear();

        if (j.contains("colors-points"))
        {
            for (const auto& colorPt : j.at("colors-points"))
            {
                g.colorPoints.push_back({
                    colorPt[0],
                    colorPt[1],
                    colorPt[2],
                    colorPt[3]
                });
            }
        }
        g.angle = j.value("angle", g.angle);
    }

    // SPRITE
    json Serializer<Sprite>::to_json(const Sprite& s, SerializeContext* ctx)
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
    void Serializer<Sprite>::from_json(Sprite& s, const json& j)
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
            s.textureID = hashed_string{j.at("texture").get<string>().c_str()}.value();
    }

    // UIIMAGE
    json Serializer<UIImage>::to_json(const UIImage& i, SerializeContext* ctx)
    {
        return json{
            { "sprite" , JSON::Serializer<Sprite>::to_json(i.sprite, ctx)   },
            { "transform"  , JSON::Serializer<UITransform>::to_json(i.transform, ctx) }
        };
    }
    void Serializer<UIImage>::from_json(UIImage& i, const json& j)
    {
        if (j.contains("sprite"))
            JSON::Serializer<Sprite>::from_json(i.sprite, j.at("sprite"));
        if (j.contains("transform"))
            JSON::Serializer<UITransform>::from_json(i.transform, j.at("transform"));
    }

    // UITEXTLABEL
    json Serializer<UITextLabel>::to_json(const UITextLabel& l, SerializeContext* ctx)
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
    void Serializer<UITextLabel>::from_json(UITextLabel& l, const json& j)
    {
        if (j.contains("frame"))
            JSON::Serializer<UIFrame>::from_json(l.frame, j.at("frame"));
        l.text = j.value("text", l.text);
        if (j.contains("font"))
            l.fontID = hashed_string{j.at("font").get<string>().c_str()}.value();
        l.fontSize = j.value("font-size", l.fontSize);
        l.spacing = j.value("spacing", l.spacing);
        
        if (j.contains("alignment"))
            JSON::Serializer<Alignment>::from_json(l.alignment, j.at("alignment"));
        if (j.contains("padding"))
            JSON::Serializer<Padding>::from_json(l.padding, j.at("padding"));
    }

    // UIBUTTON EVENT CALLBACKS
    json Serializer<UIButtonEventCallbacks>::to_json(const UIButtonEventCallbacks& c, SerializeContext* ctx)
    {
        
        return json{
            { "mouse-hover"  , SerializeEventID(c.mouseHover)  },
            { "mouse-enter"  , SerializeEventID(c.mouseEnter)  },
            { "mouse-exit"   , SerializeEventID(c.mouseExit)   },
            { "left-click"   , SerializeEventID(c.leftClick)   },
            { "middle-click" , SerializeEventID(c.middleClick) },
            { "right-click"  , SerializeEventID(c.rightClick)  }
        };
    }
    void Serializer<UIButtonEventCallbacks>::from_json(UIButtonEventCallbacks& c, const json& j)
    {
        if (j.contains("mouse-hover"))
            c.mouseHover = DeserializeEventID(j.at("mouse-hover"));

        if (j.contains("mouse-enter"))
            c.mouseEnter = DeserializeEventID(j.at("mouse-enter"));

        if (j.contains("mouse-exit"))
            c.mouseExit = DeserializeEventID(j.at("mouse-exit"));

        if (j.contains("left-click"))
            c.leftClick = DeserializeEventID(j.at("left-click"));

        if (j.contains("middle-click"))
            c.middleClick = DeserializeEventID(j.at("middle-click"));

        if (j.contains("right-click"))
            c.rightClick = DeserializeEventID(j.at("right-click"));
    }

    // UIBUTTON
    json Serializer<UIButton>::to_json(const UIButton& b, SerializeContext* ctx)
    {
        return json{
            { "event-callbacks"          , JSON::Serializer<UIButtonEventCallbacks>::to_json(b.eventCallbacks, ctx) },
            { "bounds"                   , JSON::Serializer<UITransform>::to_json(b.bounds, ctx)                    },
            { "inherits-ui" , b.inheritsUI                                                                          },
            { "active"                   , b.active                                                                 }
        };
    }
    void Serializer<UIButton>::from_json(UIButton& b, const json& j)
    {
        if (j.contains("event-callbacks"))
            JSON::Serializer<UIButtonEventCallbacks>::from_json(b.eventCallbacks, j.at("event-callbacks"));
        if (j.contains("bounds"))
            JSON::Serializer<UITransform>::from_json(b.bounds, j.at("bounds"));

        b.inheritsUI = j.value("inherits-ui", b.inheritsUI);
        b.active                 = j.value("active", b.active);
    }

    // TAGS
    json Serializer<Tags>::to_json(
        const Tags& t,
        SerializeContext* ctx
    )
    {
        json tags = json::array();

        for (auto tag : t.tags)
            tags.push_back(SerializeEventID(tag));

        return {
            {"tags", std::move(tags)}
        };
    }

    void Serializer<Tags>::from_json(
        Tags& t,
        const json& j
    )
    {
        if (!j.contains("tags"))
            return;

        t.tags.clear();

        for (const auto& value : j.at("tags"))
            t.tags.insert(DeserializeEventID(value));
    }

    // PARAMETER
    json Serializer<Parameter>::to_json(const Parameter& param, SerializeContext* ctx)
    {
        return json{
            {
                GET_FORMER_HASHED(param.id),
                visit([](const auto& actualValue) -> json
                {
                    return actualValue;
                }, param.value)
            },
            {
                "active",
                param.active
            }
        };
    }
    void Serializer<Parameter>::from_json(Parameter& param, const json& j)
    {
        for (const auto& [key, val] : j.items())
        {
            if (key == "active")
            {
                param.active = val.get<bool>();
                continue;
            }
            
            param.id = HASH_ID(key.c_str());
            if (val.is_boolean())
                param.value = val.get<bool>();
            else if (val.is_number_integer())
                param.value = val.get<int>();
            else if (val.is_number_float())
                param.value = val.get<float>();
            else if (val.is_string())
                param.value = val.get<string>();
            else
                dbgln("Unsupported parameter type: " + val.dump(), LogType::WARNING);
        }
    }

    // PARAMETERS
    json Serializer<Parameters>::to_json(const Parameters& p, SerializeContext* ctx)
    {
        vector<json> parameters;
        for (const auto& param : p.parameters)
        {
            auto paramJson = JSON::Serializer<Parameter>::to_json(param, ctx);
            parameters.push_back(paramJson);
        }

        return json{
            { "values" , parameters }
        };
    }
    void Serializer<Parameters>::from_json(Parameters& p, const json& j)
    {
        if (j.contains("values"))
        {
            vector<Parameter> params;
            for (const auto& paramJson : j["values"])
            {
                Parameter param;
                JSON::Serializer<Parameter>::from_json(param, paramJson);
                params.push_back(std::move(param));
            }

            p.parameters = std::move(params);
        }
    }

    // SCRIPT
    json Serializer<Script>::to_json(
        const Script& s,
        SerializeContext* ctx
    )
    {
        auto serializeFunctions = [](const auto& functions) -> json
        {
            json result = json::array();

            for (auto id : functions)
                result.push_back(SerializeEventID(id));

            return result;
        };

        return json{
            {
                "functions",
                {
                    {"create", serializeFunctions(s.onCreateFunctions)},
                    {"update", serializeFunctions(s.onUpdateFunctions)},
                    {"destroy", serializeFunctions(s.onDestroyFunctions)}
                }
            },
            {"active", s.active},
            {"initiated", s.initiated}
        };
    }

    void Serializer<Script>::from_json(
        Script& s,
        const json& j
    )
    {
        if (j.contains("functions"))
        {
            const auto& functions = j.at("functions");

            auto deserializeFunctions = [](
                const json& source,
                const char* key,
                auto& destination
            )
            {
                if (!source.contains(key))
                    return;

                destination.clear();

                for (const auto& value : source.at(key))
                    destination.push_back(DeserializeEventID(value));
            };

            deserializeFunctions(
                functions,
                "create",
                s.onCreateFunctions
            );

            deserializeFunctions(
                functions,
                "update",
                s.onUpdateFunctions
            );

            deserializeFunctions(
                functions,
                "destroy",
                s.onDestroyFunctions
            );
        }

        s.active = j.value("active", s.active);
        s.initiated = j.value("initiated", s.initiated);
    }
}