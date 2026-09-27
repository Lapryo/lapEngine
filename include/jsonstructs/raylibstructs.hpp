#pragma once

#include "eutil.hpp"

namespace lapCore
{
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
    struct JSON::Serializer<Rectangle>
    {
        static json to_json(const Rectangle& r, SerializeContext* ctx = nullptr)
        {
            return json{
                {"position", JSON::Serializer<Vector2>::to_json({r.x, r.y})},
                {"size", JSON::Serializer<Vector2>::to_json({r.width, r.height})}
            };
        }

        static void from_json(Rectangle& r, const json& j)
        {
            if (j.contains("position")) {
                r.x = j.at("position").at(0).get<float>();
                r.y = j.at("position").at(1).get<float>();
            }

            if (j.contains("size")) {
                r.width = j.at("size").at(0).get<float>();
                r.height = j.at("size").at(1).get<float>();
            }
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
}