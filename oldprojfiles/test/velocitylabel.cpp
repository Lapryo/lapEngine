#include "hub.hpp"
using namespace lapCore;
using namespace lapHub;

namespace VelocityLabelObject {
    const bool updatevelocitylabel = [] {
        ScriptRegistry::onUpdateFunctions["updatevelocitylabel"] = [](Scene* scene, entt::entity entity, float dt) {
            entt::entity box = scene->FindEntity("cube");
            Transform2D &boxTransform = scene->entities.get<Transform2D>(box);

            std::string velocityY_tostring = std::to_string(boxTransform.velocity.y);
            scene->entities.get<TextLabel>(entity).text = "BOX'S Y VEL: " + velocityY_tostring;
        };
        return true;
    }();
}