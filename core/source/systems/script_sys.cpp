#include "systems.hpp"
#include "scriptregistry.hpp"
#include "scene.hpp"

using namespace lapCore;

void ScriptSystem::Update(float deltaTime, entt::registry &registry)
{
    auto view = registry.view<Script>();

    for (auto entity : view)
    {
        auto &script = view.get<Script>(entity);
        if (!script.active)
            continue;

        if (!script.initiated)
        {
            auto createFunc = ScriptRegistry::onCreateFunctions[script.onCreateFunction];

            if (createFunc)
                createFunc(scene, entity);
            script.initiated = true;
        }

        auto updateFunc = ScriptRegistry::onUpdateFunctions[script.onUpdateFunction];
        if (updateFunc)
            updateFunc(scene, entity, deltaTime);
    }
}

void ScriptSystem::OnDestroy(entt::registry &registry)
{
    auto view = registry.view<Script>();
    for (auto entity : view)
    {
        auto &script = view.get<Script>(entity);
        if (!script.active)
            continue;

        auto destroyFunc = ScriptRegistry::onDestroyFunctions[script.onDestroyFunction];
        if (destroyFunc)
            destroyFunc(scene, entity);
    }
}