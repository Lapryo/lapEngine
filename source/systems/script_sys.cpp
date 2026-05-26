#include "core.hpp"

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
            for (const auto &func : script.onCreateFunctions)
                EventRegistry::Fire(func);
            script.initiated = true;
        }

        for (const auto &func : script.onUpdateFunctions)
            EventRegistry::Fire(func, deltaTime);
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

        for (const auto &func : script.onDestroyFunctions)
            EventRegistry::Fire(func);
    }
}