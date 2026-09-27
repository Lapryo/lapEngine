#pragma once

#include "eutil.hpp"

namespace lapCore
{
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
            else if (hitboxType == "circle")
            {
                p = b2MakeRoundedBox(j.value("radius", 1.f) / 2.f, j.value("radius", 1.f) / 2.f, j.value("radius", 1.f) / 2.f);
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
}