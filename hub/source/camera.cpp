#include "hub.hpp"
using namespace lapCore;
using namespace lapHub;

namespace CameraObject {
    const bool followbox = [] {
        ScriptRegistry::onUpdateFunctions["followbox"] = [](Scene* scene, entt::entity entity, float dt) {
            entt::entity box = scene->FindEntity("cube");

            Vector2 boxPos = scene->entities.get<Transform2D>(box).position;
            boxPos.x -= 6400/2;
            boxPos.y -= 4800/2;

            Camera2D& camTarget = scene->entities.get<Cam2D>(entity).camera;
            const float LERP_SMOOTHING = 0.5f;

            camTarget.target.x += (boxPos.x - camTarget.target.x) * LERP_SMOOTHING * dt;
            camTarget.target.y += (boxPos.y - camTarget.target.y) * LERP_SMOOTHING * dt;
        };
        return true;
    }();
};