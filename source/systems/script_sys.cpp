#include "systems/script_sys.hpp"
#include "elements.hpp"
#include "event.hpp"

using namespace lapCore;
using namespace ELEMENTS::Other;

void ScriptSystem::Update(float deltaTime, registry &registry)
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
                EventRegistry::Fire(scene, func);
            script.initiated = true;
        }

        for (const auto &func : script.onUpdateFunctions)
            EventRegistry::Fire(scene, func, deltaTime);
    }
}

void ScriptSystem::OnDestroy(registry &registry)
{
    auto view = registry.view<Script>();
    for (auto entity : view)
    {
        auto &script = view.get<Script>(entity);
        if (!script.active)
            continue;

        for (const auto &func : script.onDestroyFunctions)
            EventRegistry::Fire(scene, func);
    }
}