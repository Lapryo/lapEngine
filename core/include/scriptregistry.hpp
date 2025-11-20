#ifndef SCRIPT_REGISTRY_HPP
#define SCRIPT_REGISTRY_HPP

#include "entt/entt.hpp"
#include "scene.hpp"

using namespace lapCore;

struct ScriptRegistry
{
    inline static std::unordered_map<std::string, std::function<void(Scene*, Object&)>> onCreateFunctions = {
        {"", [](Scene*, Object) {}}
    };
    inline static std::unordered_map<std::string, std::function<void(Scene*, Object&, float)>> onUpdateFunctions = {
        {"", [](Scene*, Object, float) {}}
    };
    inline static std::unordered_map<std::string, std::function<void(Scene*, Object&)>> onDestroyFunctions = {
        {"", [](Scene*, Object) {}}
    };
};

#endif