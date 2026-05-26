#ifndef EUTIL_HPP
#define EUTIL_HPP

#include <raylib.h>
#include <nlohmann/json.hpp>
#include <box2d/box2d.h>

#include "reflection.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace lapCore
{
    #define HASH(name) entt::hashed_string{name}
    #define HASH_ID(name) HASH(name).value()

    template <>
    struct JSON::Serializer<Vector2>
    {
        static json to_json(const Vector2& v, SerializeContext* ctx = nullptr)
        {
            return json{
                v.x, v.y
            };
        }

        static void from_json(Vector2& v, const json& j)
        {
            v.x = j.at(0).get<float>();
            v.y = j.at(1).get<float>();
        }
    };

    template <>
    struct JSON::Serializer<Color>
    {
        static json to_json(const Color& c, SerializeContext* ctx = nullptr)
        {
            return json{
                c.r, c.g, c.b, c.a
            };
        }

        static void from_json(Color& c, const json& j)
        {
            c.r = j.at(0).get<unsigned char>();
            c.g = j.at(1).get<unsigned char>();
            c.b = j.at(2).get<unsigned char>();
            c.a = j.at(3).get<unsigned char>();
        }
    };

    template <>
    struct JSON::Serializer<Camera2D>
    {
        static json to_json(const Camera2D& cam, SerializeContext* ctx = nullptr)
        {
            return json{
                {"offset", JSON::Serializer<Vector2>::to_json(cam.offset, ctx) },
                {"target", JSON::Serializer<Vector2>::to_json(cam.target, ctx) },
                {"rotation", cam.rotation},
                {"zoom", cam.zoom}
            };
        }

        static void from_json(Camera2D& cam, const json& j)
        {
            JSON::Serializer<Vector2>::from_json(cam.offset, j.at("offset"));
            JSON::Serializer<Vector2>::from_json(cam.target, j.at("target"));
            cam.rotation = j.value("rotation", cam.rotation);
            cam.zoom = j.value("zoom", cam.zoom);
        }
    };

    template <>
    struct JSON::Serializer<b2BodyType>
    {
        static json to_json(const b2BodyType& t, SerializeContext* ctx = nullptr)
        {
            switch (t)
            {
                case b2_kinematicBody:
                    return "kinematic";
                case b2_dynamicBody:
                    return "dynamic";
                default:
                    return "static";
            }
        }

        static void from_json(b2BodyType& t, const json& j)
        {
            const auto val = j.get<std::string>();

            if (val == "kinematic")
                t = b2_kinematicBody;
            else if (val == "dynamic")
                t = b2_dynamicBody;
            else
                t = b2_staticBody;
        }
    };

    template <>
    struct JSON::Serializer<b2Vec2>
    {
        static json to_json(const b2Vec2& v, SerializeContext* ctx = nullptr)
        {
            return json{
                v.x, v.y
            };
        }

        static void from_json(b2Vec2& v, const json& j)
        {
            v.x = j.at(0).get<float>();
            v.y = j.at(1).get<float>();
        }
    };

    template <>
    struct JSON::Serializer<b2Rot>
    {
        static json to_json(const b2Rot& r, SerializeContext* ctx = nullptr)
        {
            return json{
                {"cosine", r.c},
                {"sine", r.s}
            };
        }

        static void from_json(b2Rot& r, const json& j)
        {
            r = b2Rot{
                j.value("cosine", r.c),
                j.value("sine", r.s)
            };
        }
    };

    template <>
    struct JSON::Serializer<b2MotionLocks>
    {
        static json to_json(const b2MotionLocks& m, SerializeContext* ctx = nullptr)
        {
            return json{
                {"lock-linear-x", m.linearX},
                {"lock-linear-y", m.linearY},
                {"lock-angular-z", m.angularZ}
            };
        }

        static void from_json(b2MotionLocks& m, const json& j)
        {
            m.linearX = j.value("lock-linear-x", m.linearX);
            m.linearY = j.value("lock-linear-y", m.linearY);
            m.angularZ = j.value("lock-angular-z", m.angularZ);
        }
    };

    template<>
    struct JSON::Serializer<b2BodyDef>
    {
        static json to_json(const b2BodyDef& def, SerializeContext* ctx = nullptr)
        {
            return json{
                {"type", JSON::Serializer<b2BodyType>::to_json(def.type, ctx)},
                {"allow-fast-rotation", def.allowFastRotation},
                {"linear-damping", def.linearDamping},
                {"angular-damping", def.angularDamping},
                {"enable-sleep", def.enableSleep},
                {"sleep-threshold", def.sleepThreshold},
                {"is-awake", def.isAwake},
                {"initial-rotation", JSON::Serializer<b2Rot>::to_json(def.rotation, ctx)},
                {"is-bullet", def.isBullet},
                {"debug-name", def.name},
                {"initial-position", JSON::Serializer<b2Vec2>::to_json(def.position, ctx)},
                {"gravity-scale", def.gravityScale},
                {"motion-locks", JSON::Serializer<b2MotionLocks>::to_json(def.motionLocks, ctx)},
                {"is-enabled", def.isEnabled},
                {"initial-velocity", JSON::Serializer<b2Vec2>::to_json(def.linearVelocity, ctx)},
                {"initial-angular-velocity", def.angularVelocity}
            };
        }

        static void from_json(b2BodyDef& def, const json& j)
        {
            if (j.contains("type"))
                JSON::Serializer<b2BodyType>::from_json(def.type, j.at("type"));
            
            def.allowFastRotation = j.value("allow-fast-rotation", def.allowFastRotation);
            def.linearDamping = j.value("linear-damping", def.linearDamping);
            def.angularDamping = j.value("angular-damping", def.angularDamping);
            def.enableSleep = j.value("enable-sleep", def.enableSleep);
            def.sleepThreshold = j.value("sleep-threshold", def.sleepThreshold);
            def.isAwake = j.value("is-awake", def.isAwake);
            if (j.contains("initial-rotation"))
                JSON::Serializer<b2Rot>::from_json(def.rotation, j.at("initial-rotation"));

            def.isBullet = j.value("is-bullet", def.isBullet);
            if (j.contains("initial-position"))
                JSON::Serializer<b2Vec2>::from_json(def.position, j.at("initial-position"));
            def.gravityScale = j.value("gravity-scale", def.gravityScale);
            if (j.contains("motion-locks"))
                JSON::Serializer<b2MotionLocks>::from_json(def.motionLocks, j.at("motion-locks"));
            def.isEnabled = j.value("is-enabled", def.isEnabled);
            if (j.contains("initial-velocity"))
                JSON::Serializer<b2Vec2>::from_json(def.linearVelocity, j.at("initial-velocity"));
            def.angularVelocity = j.value("initial-angular-velocity", def.angularVelocity);
        }
    };

    template <>
    struct JSON::Serializer<b2Filter>
    {
        static json to_json(const b2Filter& f, SerializeContext* ctx = nullptr)
        {
            return json{
                {"category-bits", f.categoryBits},
                {"mask-bits", f.maskBits},
                {"group-index", f.groupIndex}
            };
        }

        static void from_json(b2Filter& f, const json& j)
        {
            f.categoryBits = j.value("category-bits", f.categoryBits);
            f.maskBits = j.value("mask-bits", f.maskBits);
            f.groupIndex = j.value("group-index", f.groupIndex);
        }
    };

    template <>
    struct JSON::Serializer<b2SurfaceMaterial>
    {
        static json to_json(const b2SurfaceMaterial& m, SerializeContext* ctx = nullptr)
        {
            return json{
                {"friction", m.friction},
                {"restitution", m.restitution},
                {"custom-color", m.customColor},
                {"rolling-resistance", m.rollingResistance},
                {"tangent-speed", m.tangentSpeed},
                {"user-material-id", m.userMaterialId}
            };
        }

        static void from_json(b2SurfaceMaterial& m, const json& j)
        {
            m.friction = j.value("friction", 0.f);
            m.restitution = j.value("restitution", 0.f);
            m.customColor = j.value("custom-color", 0xFFFFFFFF);
            m.rollingResistance = j.value("rolling-resistance", 0.f);
            m.tangentSpeed = j.value("tangent-speed", 0.f);
            m.userMaterialId = j.value("user-material-id", 0);
        }
    };

    template <>
    struct JSON::Serializer<b2ShapeDef>
    {
        static json to_json(const b2ShapeDef& def, SerializeContext* ctx = nullptr)
        {
            return json{
                {"density", def.density},
                {"enable-contact-events", def.enableContactEvents},
                {"enable-custom-filtering", def.enableCustomFiltering},
                {"enable-hit-events", def.enableHitEvents},
                {"enable-sensor-events", def.enableSensorEvents},
                {"filter", JSON::Serializer<b2Filter>::to_json(def.filter)},
                {"invoke-contact-creation", def.invokeContactCreation},
                {"is-sensor", def.isSensor},
                {"material", JSON::Serializer<b2SurfaceMaterial>::to_json(def.material, ctx)},
                {"update-body-mass", def.updateBodyMass}
            };
        }

        static void from_json(b2ShapeDef& def, const json& j)
        {
            def.density = j.value("density", 0.f);
            def.enableContactEvents = j.value("enable-contact-events", false);
            def.enableCustomFiltering = j.value("enable-custom-filtering", false);
            def.enableHitEvents = j.value("enable-hit-events", false);
            def.enableSensorEvents = j.value("enable-sensor-events", false);
            if (j.contains("filter"))
                JSON::Serializer<b2Filter>::from_json(def.filter, j.at("filter"));
            def.invokeContactCreation = j.value("invoke-contact-creation", false);
            def.isSensor = j.value("is-sensor", false);
            if (j.contains("material"))
                JSON::Serializer<b2SurfaceMaterial>::from_json(def.material, j.at("material"));
            def.updateBodyMass = j.value("update-body-mass", true);
        }
    };

    template <>
    struct JSON::Serializer<b2Polygon>
    {
        static json to_json(const b2Polygon& p, SerializeContext* ctx = nullptr)
        {
            std::vector<float> vertices;
            std::vector<float> normals;

            for (int i = 0; i < p.count; i++)
            {
                vertices.push_back(p.vertices[i].x);
                vertices.push_back(p.vertices[i].y);
                normals.push_back(p.normals[i].x);
                normals.push_back(p.normals[i].y);
            }

            return json{
                {"centroid", JSON::Serializer<b2Vec2>::to_json(p.centroid)},
                {"radius", p.radius},
                {"count", p.count},
                {"vertices", vertices},
                {"normals", normals},
                {"hitbox-type", "export"}
            };
        }

        static void from_json(b2Polygon& p, const json& j)
        {
            std::string hitboxType = j.value("hitbox-type", "box");
            if (hitboxType == "box")
            {
                p = b2MakeBox(j.value("width", 1.f) / 2.f, j.value("height", 1.f) / 2.f);
            }
            else if (hitboxType == "bounding-box")
            {
                Model model = LoadModel(j.value("model-path", std::string()).c_str());
                BoundingBox box = GetModelBoundingBox(model);
                p = b2MakeBox((box.max.x - box.min.x) / 2.f, (box.max.y - box.min.y) / 2.f);
            }
            else if (hitboxType == "mesh")
            {
                // get all the meshes of a model into one
                Model model = LoadModel(j.value("asset", "").c_str());

                // 1. Calculate total vertices to allocate the correct amount of memory
                int totalVertices = 0;
                for (int i = 0; i < model.meshCount; i++) {
                    totalVertices += model.meshes[i].vertexCount;
                }

                // 2. Allocate the b2Vec2 array
                b2Vec2* points = new b2Vec2[totalVertices];
                int current_point_index = 0;

                // 3. Loop through meshes and vertices
                for (int mi = 0; mi < model.meshCount; mi++) {
                    Mesh mesh = model.meshes[mi];
                    
                    for (int vi = 0; vi < mesh.vertexCount; vi++) {
                        // raylib vertices are float* (x, y, z)
                        // We multiply index by 3 to get the start of each vertex triplet
                        float x = mesh.vertices[vi * 3];
                        float y = mesh.vertices[vi * 3 + 1];

                        // 4. Assign to your Box2D vector (ignoring Z for 2D physics)
                        points[current_point_index] = b2Vec2{x, y};
                        current_point_index++;
                    }
                }

                b2Hull hull = b2ComputeHull(points, totalVertices);

                delete[] points;
                UnloadModel(model);

                p = b2MakePolygon(&hull, j.value("radius", 1.f));
            }
            else if (hitboxType == "custom")
            {
                auto j_points = j.at("points");
                auto pointCount = j_points.size();
                int totalVertices = pointCount / 3;
                b2Vec2* points = new b2Vec2[pointCount];

                for (int i = 0; i < totalVertices; i++) {
                    // Use .get<float>() to safely extract numbers
                    float x = j_points[i * 3].get<float>();
                    float y = j_points[i * 3 + 1].get<float>();

                    points[i] = b2Vec2{x, y};
                }

                b2Hull hull = b2ComputeHull(points, totalVertices);
                delete[] points;
                
                p = b2MakePolygon(&hull, j.value("radius", 1.f));
            }
            else if (hitboxType == "export")
            {
                if (j.contains("centroid"))
                    JSON::Serializer<b2Vec2>::from_json(p.centroid, j.at("centroid"));
                p.radius = j.value("radius", 0.f);
                p.count = j.value("count", 0);
                std::vector<float> vertices = j.value("vertices", std::vector<float>());
                std::vector<float> normals = j.value("normals", std::vector<float>());
                for (int i = 0; i < p.count; i++)
                {
                    p.vertices[i] = b2Vec2{vertices[i * 2], vertices[i * 2 + 1]};
                    p.normals[i] = b2Vec2{normals[i * 2], normals[i * 2 + 1]};
                }
            }
        }
    };

    struct Renderable
    {
        // Not to be changed, i mean you can, but it wont do anything, itll just be overwritten by the render system
        Rectangle drawRect;

        unsigned int zlayer;
        bool isScreenSpace;
        bool visible = true;
        Color tint;

        bool usesUIListVisiblity;
    };
    template <>
    struct JSON::Serializer<Renderable>
    {
        static json to_json(const Renderable& r, SerializeContext* ctx = nullptr)
        {
            return json{
                {"z-layer", r.zlayer},
                {"is-screen-space", r.isScreenSpace},
                {"visible", r.visible},
                {"tint", JSON::Serializer<Color>::to_json(r.tint, ctx)},
                {"inherits-ui", r.usesUIListVisiblity}
            };
        }

        static void from_json(Renderable& r, const json& j)
        {
            r.zlayer = j.value("z-layer", r.zlayer);
            r.isScreenSpace = j.value("is-screen-space", r.isScreenSpace);
            r.visible = j.value("visible", r.visible);
            r.usesUIListVisiblity = j.value("inherits-ui", r.usesUIListVisiblity);

            if (j.contains("tint"))
                JSON::Serializer<Color>::from_json(r.tint, j.at("tint"));
        }
    };

    enum class Axis2D
    {
        HORIZONTAL,
        VERTICAL
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
    
    struct ScrollSettings
    {
        Axis2D direction;
        bool allowHorizontalScrolling;
        bool allowVerticalScrolling;
        float scrollSpeed;
    };
    template <>
    struct JSON::Serializer<ScrollSettings>
    {
        static json to_json(const ScrollSettings& s, SerializeContext* ctx = nullptr)
        {
            return json{
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

            s.allowHorizontalScrolling = j.value("horizontal-scrolling", true);
            s.allowVerticalScrolling = j.value("vertical-scrolling", true);
            s.scrollSpeed = j.value("scroll-speed", 1.f);
        }
    };

    struct Animated
    {
        bool active;
        unsigned int index;
        std::vector<float> frameTimes;
    };
    template <>
    struct JSON::Serializer<Animated>
    {
        static json to_json(const Animated& a, SerializeContext* ctx = nullptr)
        {
            return json{
                {"active", a.active},
                {"index", a.index},
                {"frame-times", a.frameTimes}
            };
        }
        static void from_json(Animated& a, const json& j)
        {
            a.active = j.value("active", false);
            a.index = j.value("index", 0);
            a.frameTimes = j.value("frame-times", std::vector<float>());
        }
    };

    enum class HorizontalAlignment
    {
        LEFT,
        MIDDLE,
        RIGHT
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

    enum class VerticalAlignment
    {
        TOP,
        MIDDLE,
        BOTTOM
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

    enum class Direction2D
    {
        UP,
        DOWN,
        LEFT,
        RIGHT
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

    struct Alignment
    {
        HorizontalAlignment horizontal;
        VerticalAlignment vertical;
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

    struct FrameVector
    {
        Vector2 scale;
        Vector2 offset;
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

    struct Padding
    {
        float top, bottom, left, right;
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

    struct UIOrigin
    {
        FrameVector position;
        FrameVector size;
        Vector2 anchor;
        float rotation;
    };
    template <>
    struct JSON::Serializer<UIOrigin>
    {
        static json to_json(const UIOrigin& o, SerializeContext* ctx = nullptr)
        {
            return json{
                {"position", JSON::Serializer<FrameVector>::to_json(o.position, ctx)},
                {"size", JSON::Serializer<FrameVector>::to_json(o.size, ctx)},
                {"anchor", JSON::Serializer<Vector2>::to_json(o.anchor, ctx)},
                {"rotation", o.rotation}
            };
        }

        static void from_json(UIOrigin& o, const json& j)
        {
            if (j.contains("position"))
                JSON::Serializer<FrameVector>::from_json(o.position, j.at("position"));
            if (j.contains("size"))
                JSON::Serializer<FrameVector>::from_json(o.size, j.at("size"));
            if (j.contains("anchor"))
                JSON::Serializer<Vector2>::from_json(o.anchor, j.at("anchor"));
            o.rotation = j.value("rotation", 0.f);
        }
    };

    std::string ReadFileToString(const std::string &filePath);
    void WriteStringToFile(const std::string &filePath, const std::string &data);

    nlohmann::json ReadFileToJsonObject(const std::string &filePath);

    Vector2 GetMouseInViewportSpace(int logicalWidth, int logicalHeight);

    Rectangle UIOriginToRect(UIOrigin origin, int logicalWidth, int logicalHeight);
    Vector2 FrameVectorToVec2(lapCore::FrameVector vector, int logicalWidth, int logicalHeight);

    std::filesystem::path GetExecutableDir();

    enum class LogType
    {
        INFO,
        NOTICE,
        WARNING,
        ERROR
    };
    void dbgln(const std::string &message, LogType type = LogType::INFO);
}

#endif