#include "reflection.hpp"
#include "json/structs/raylib.hpp"

namespace lapCore::JSON
{
    // VECTOR2
    json Serializer<Vector2>::to_json(const Vector2& v, SerializeContext* ctx)
    {
        return json{
            v.x, v.y
        };
    }
    void Serializer<Vector2>::from_json(Vector2& v, const json& j)
    {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
    }

    // RECTANGLE
    json Serializer<Rectangle>::to_json(const Rectangle& r, SerializeContext* ctx)
    {
        return json{
            {"position", Serializer<Vector2>::to_json({r.x, r.y})},
            {"size", Serializer<Vector2>::to_json({r.width, r.height})}
        };
    }
    void Serializer<Rectangle>::from_json(Rectangle& r, const json& j)
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

    // COLOR
    json Serializer<Color>::to_json(const Color& c, SerializeContext* ctx)
    {
        return json{
            c.r, c.g, c.b, c.a
        };
    }
    void Serializer<Color>::from_json(Color& c, const json& j)
    {
        c.r = j.at(0).get<unsigned char>();
        c.g = j.at(1).get<unsigned char>();
        c.b = j.at(2).get<unsigned char>();
        c.a = j.at(3).get<unsigned char>();
    }

    // CAMERA2D
    json Serializer<Camera2D>::to_json(const Camera2D& cam, SerializeContext* ctx)
    {
        return json{
            {"offset", Serializer<Vector2>::to_json(cam.offset, ctx) },
            {"target", Serializer<Vector2>::to_json(cam.target, ctx) },
            {"rotation", cam.rotation},
            {"zoom", cam.zoom}
        };
    }
    void Serializer<Camera2D>::from_json(Camera2D& cam, const json& j)
    {
        Serializer<Vector2>::from_json(cam.offset, j.at("offset"));
        Serializer<Vector2>::from_json(cam.target, j.at("target"));
        cam.rotation = j.value("rotation", cam.rotation);
        cam.zoom = j.value("zoom", cam.zoom);
    }
}