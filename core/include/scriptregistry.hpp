#ifndef SCRIPT_REGISTRY_HPP
#define SCRIPT_REGISTRY_HPP

#include "entt/entt.hpp"
#include "scene.hpp"

using namespace lapCore;

struct ScriptRegistry
{
    inline static std::unordered_map<std::string, std::function<void(Scene*, entt::entity)>> onCreateFunctions = {
        {"", [](Scene*, entt::entity) {}}
    };
    inline static std::unordered_map<std::string, std::function<void(Scene*, entt::entity, float)>> onUpdateFunctions = {
        {"", [](Scene*, entt::entity, float) {}}
    };
    inline static std::unordered_map<std::string, std::function<void(Scene*, entt::entity)>> onDestroyFunctions = {
        {"", [](Scene*, entt::entity) {}}
    };
};

#endif