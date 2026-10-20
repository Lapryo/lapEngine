#pragma once

#include "eutil.hpp"

namespace lapCore::JSON
{
    template <>
    struct Serializer<b2BodyType>
    {
        static json to_json(const b2BodyType& t, SerializeContext* ctx = nullptr);
        static void from_json(b2BodyType& t, const json& j);
    };

    template <>
    struct Serializer<b2Vec2>
    {
        static json to_json(const b2Vec2& v, SerializeContext* ctx = nullptr);
        static void from_json(b2Vec2& v, const json& j);
    };

    template <>
    struct Serializer<b2Rot>
    {
        static json to_json(const b2Rot& r, SerializeContext* ctx = nullptr);
        static void from_json(b2Rot& r, const json& j);
    };

    template <>
    struct Serializer<b2MotionLocks>
    {
        static json to_json(const b2MotionLocks& m, SerializeContext* ctx = nullptr);
        static void from_json(b2MotionLocks& m, const json& j);
    };

    template<>
    struct Serializer<b2BodyDef>
    {
        static json to_json(const b2BodyDef& def, SerializeContext* ctx = nullptr);
        static void from_json(b2BodyDef& def, const json& j);
    };

    template <>
    struct Serializer<b2Filter>
    {
        static json to_json(const b2Filter& f, SerializeContext* ctx = nullptr);
        static void from_json(b2Filter& f, const json& j);
    };

    template <>
    struct Serializer<b2SurfaceMaterial>
    {
        static json to_json(const b2SurfaceMaterial& m, SerializeContext* ctx = nullptr);
        static void from_json(b2SurfaceMaterial& m, const json& j);
    };

    template <>
    struct Serializer<b2ShapeDef>
    {
        static json to_json(const b2ShapeDef& def, SerializeContext* ctx = nullptr);
        static void from_json(b2ShapeDef& def, const json& j);
    };

    template <>
    struct Serializer<b2Polygon>
    {
        static json to_json(const b2Polygon& p, SerializeContext* ctx = nullptr);
        static void from_json(b2Polygon& p, const json& j);
    };
}