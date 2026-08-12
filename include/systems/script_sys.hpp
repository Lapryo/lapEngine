#pragma once

#include "system.hpp"
using namespace lapCore;

class ScriptSystem : public System
{
public:
    ScriptSystem(
        Scene *scene, 
        unsigned int order
    ) : System(order, scene) {}

    void Update(
        float deltaTime, 
        entt::registry &registry
    ) override;

    void OnDestroy(entt::registry &registry);

    template <typename... Args>
    void SpawnUpdateScript(entt::id_type objectID, entt::hashed_string eventName, void (*func)(Scene*, entt::id_type, entt::id_type, Args...))
    {
        auto object = scene->FindObject(objectID);
        if (!object) return;

        auto* script = scene->FindElement<Script>(*object);
        if (script)
        {
            script->onUpdateFunctions.push_back(eventName.value());
        }
        else
        {
            Script newScript;
            newScript.onUpdateFunctions.push_back(eventName.value());

            scene->AddElement<Script>(*object, newScript);
        }

        ConnectECSEvent<Args...>(objectID, eventName, func);
    }

    std::string GetName() const override 
    { return "ScriptSystem"; }
};