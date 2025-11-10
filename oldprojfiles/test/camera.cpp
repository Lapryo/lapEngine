#include "hub.hpp"
#include <algorithm>

using namespace lapCore;
using namespace lapHub;

namespace CameraObject {
    const bool followbox = [] {
        ScriptRegistry::onUpdateFunctions["followbox"] = [](Scene* scene, entt::entity entity, float dt) {
            entt::entity cube = scene->FindEntity("cube");
            if (!scene->entities.valid(cube) || !scene->entities.all_of<Transform2D>(cube))
                return;

            auto &cubeTransform = scene->entities.get<Transform2D>(cube);
            auto &cam = scene->entities.get<Cam2D>(entity).camera;

            const Vector2 logicalRes = scene->logicalResolution;
            const float LERP_SMOOTHING = 1.25f;
            const float MIN_SPEED = 50.0f;   // adjust as needed (units per second)
            const float MAX_SPEED = 2500.0f; // optional — cap to prevent snapping

            Vector2 targetPos = {
                cubeTransform.position.x - logicalRes.x * 0.5f,
                cubeTransform.position.y - logicalRes.y * 0.5f
            };

            // compute smooth delta
            Vector2 delta = {
                (targetPos.x - cam.target.x) * LERP_SMOOTHING * dt,
                (targetPos.y - cam.target.y) * LERP_SMOOTHING * dt
            };

            // calculate the length of the delta
            float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

            // if the delta is *smaller* than the minimum speed, normalize and scale it up
            if (dist < MIN_SPEED * dt && dist > 0.0f) {
                float scale = (MIN_SPEED * dt) / dist;
                delta.x *= scale;
                delta.y *= scale;
            }

            // optionally cap max speed so it doesn’t jump too fast
            if (dist > MAX_SPEED * dt) {
                float scale = (MAX_SPEED * dt) / dist;
                delta.x *= scale;
                delta.y *= scale;
            }

            // apply
            cam.target.x += delta.x;
            cam.target.y += delta.y;
        };
        return true;
    }();
};