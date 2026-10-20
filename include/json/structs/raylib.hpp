#pragma once

#include "eutil.hpp"

namespace lapCore::JSON
{
    template <>
    struct Serializer<Vector2>
    {
        static json to_json(const Vector2& v, SerializeContext* ctx = nullptr);
        static void from_json(Vector2& v, const json& j);
    };

    template <>
    struct Serializer<Rectangle>
    {
        static json to_json(const Rectangle& r, SerializeContext* ctx = nullptr);
        static void from_json(Rectangle& r, const json& j);
    };

    template <>
    struct Serializer<Color>
    {
        static json to_json(const Color& c, SerializeContext* ctx = nullptr);
        static void from_json(Color& c, const json& j);
    };

    template <>
    struct Serializer<Camera2D>
    {
        static json to_json(const Camera2D& cam, SerializeContext* ctx = nullptr);
        static void from_json(Camera2D& cam, const json& j);
    };
}